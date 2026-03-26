#!/usr/bin/env python3
"""Live RFC959 PASV+RETR test against ./myftp.

Flow:
- create a unique source file in FTP root
- start local myftp server
- USER/PASS login
- PASV
- open data socket to advertised endpoint
- RETR unique file over data socket
- verify received payload
"""

from __future__ import annotations

import argparse
import os
import random
import re
import socket
import subprocess
import sys
import time
import uuid
from pathlib import Path

PASV_RE = re.compile(r"\((\d+),(\d+),(\d+),(\d+),(\d+),(\d+)\)")


class TestError(RuntimeError):
    pass


def pick_free_port() -> int:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(("127.0.0.1", 0))
        return int(s.getsockname()[1])


def read_response(ctrl_file) -> tuple[int, list[str]]:
    line = ctrl_file.readline()
    if not line:
        raise TestError("Connexion controle fermee sans reponse FTP")
    line = line.decode("utf-8", errors="replace")
    lines = [line.rstrip("\r\n")]
    if len(line) < 3 or not line[:3].isdigit():
        raise TestError(f"Reponse FTP invalide: {line!r}")

    code = int(line[:3])
    if len(line) >= 4 and line[3] == "-":
        target = f"{code} "
        while True:
            next_line = ctrl_file.readline()
            if not next_line:
                raise TestError("Fin de flux pendant reponse multi-lignes")
            decoded = next_line.decode("utf-8", errors="replace")
            stripped = decoded.rstrip("\r\n")
            lines.append(stripped)
            if stripped.startswith(target):
                break
    return code, lines


def send_cmd(ctrl_sock: socket.socket, command: str) -> None:
    ctrl_sock.sendall((command + "\r\n").encode("ascii"))


def parse_pasv_endpoint(response_lines: list[str]) -> tuple[str, int]:
    joined = " ".join(response_lines)
    match = PASV_RE.search(joined)
    if not match:
        raise TestError(f"Impossible de parser PASV: {joined!r}")
    h1, h2, h3, h4, p1, p2 = (int(match.group(i)) for i in range(1, 7))
    if not all(0 <= x <= 255 for x in (h1, h2, h3, h4, p1, p2)):
        raise TestError(f"PASV contient des octets invalides: {joined!r}")
    return f"{h1}.{h2}.{h3}.{h4}", p1 * 256 + p2


def wait_server_ready(port: int, timeout: float = 4.0) -> None:
    deadline = time.time() + timeout
    last_exc: Exception | None = None
    while time.time() < deadline:
        try:
            with socket.create_connection(("127.0.0.1", port), timeout=0.3):
                return
        except OSError as exc:
            last_exc = exc
            time.sleep(0.05)
    raise TestError(f"Serveur non joignable sur 127.0.0.1:{port}: {last_exc}")


def terminate_process(proc: subprocess.Popen) -> None:
    if proc.poll() is not None:
        return
    proc.terminate()
    try:
        proc.wait(timeout=1.5)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.wait(timeout=1.5)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Test live RFC959: PASV + RETR contre ./myftp"
    )
    parser.add_argument("--bin", default="./myftp", help="Chemin binaire serveur")
    parser.add_argument("--root", default="./mtftp", help="Racine FTP (path argument)")
    parser.add_argument("--port", type=int, default=0, help="Port control (0 = auto)")
    args = parser.parse_args()

    server_bin = Path(args.bin).resolve()
    root_dir = Path(args.root).resolve()

    if not server_bin.exists():
        print(f"[FAIL] Binaire introuvable: {server_bin}")
        return 2
    if not root_dir.exists() or not root_dir.is_dir():
        print(f"[FAIL] Dossier racine FTP introuvable: {root_dir}")
        return 2

    control_port = args.port if args.port > 0 else pick_free_port()
    filename = f"download_{uuid.uuid4().hex[:12]}.txt"
    source_file = root_dir / filename

    source_payload = (
        "RFC959 PASV RETR test\n"
        f"time={int(time.time())}\n"
        f"nonce={random.randint(100000, 999999)}\n"
    ).encode("utf-8")
    source_file.write_bytes(source_payload)

    proc = subprocess.Popen(
        [str(server_bin), str(control_port), str(root_dir)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        preexec_fn=os.setsid,
    )

    transcript: list[str] = []

    try:
        wait_server_ready(control_port)
        ctrl = socket.create_connection(("127.0.0.1", control_port), timeout=3.0)
        ctrl.settimeout(4.0)
        ctrl_file = ctrl.makefile("rb")

        code, lines = read_response(ctrl_file)
        transcript += [f"S: {ln}" for ln in lines]
        if code != 220:
            raise TestError(f"Code attendu 220 au banner, recu {code}")

        send_cmd(ctrl, "USER Anonymous")
        transcript.append("C: USER Anonymous")
        code, lines = read_response(ctrl_file)
        transcript += [f"S: {ln}" for ln in lines]
        if code != 331:
            raise TestError(f"Code attendu 331 apres USER, recu {code}")

        send_cmd(ctrl, "PASS")
        transcript.append("C: PASS")
        code, lines = read_response(ctrl_file)
        transcript += [f"S: {ln}" for ln in lines]
        if code != 230:
            raise TestError(f"Code attendu 230 apres PASS, recu {code}")

        send_cmd(ctrl, "PASV")
        transcript.append("C: PASV")
        code, lines = read_response(ctrl_file)
        transcript += [f"S: {ln}" for ln in lines]
        if code != 227:
            raise TestError(f"Code attendu 227 apres PASV, recu {code}")
        pasv_host, pasv_port = parse_pasv_endpoint(lines)

        data_sock = socket.create_connection((pasv_host, pasv_port), timeout=4.0)
        data_sock.settimeout(4.0)

        send_cmd(ctrl, f"RETR {filename}")
        transcript.append(f"C: RETR {filename}")
        code, lines = read_response(ctrl_file)
        transcript += [f"S: {ln}" for ln in lines]
        if code not in (125, 150):
            raise TestError(f"Code attendu 125/150 avant transfer RETR, recu {code}")

        received_chunks: list[bytes] = []
        while True:
            chunk = data_sock.recv(4096)
            if not chunk:
                break
            received_chunks.append(chunk)
        data_sock.close()

        code, lines = read_response(ctrl_file)
        transcript += [f"S: {ln}" for ln in lines]
        if code != 226:
            raise TestError(f"Code attendu 226 apres RETR, recu {code}")

        send_cmd(ctrl, "QUIT")
        transcript.append("C: QUIT")
        code, lines = read_response(ctrl_file)
        transcript += [f"S: {ln}" for ln in lines]
        if code != 221:
            raise TestError(f"Code attendu 221 apres QUIT, recu {code}")

        ctrl_file.close()
        ctrl.close()

        received_payload = b"".join(received_chunks)
        expected_payload = source_payload + b"\r\n"
        if received_payload != expected_payload:
            raise TestError(
                "Contenu RETR incorrect "
                f"(attendu {len(expected_payload)} octets, recu {len(received_payload)})"
            )

        print("[PASS] Test RFC959 PASV+RETR valide")
        print(f"[INFO] Serveur: {server_bin}")
        print(f"[INFO] Racine FTP: {root_dir}")
        print(f"[INFO] Port controle: {control_port}")
        print(f"[INFO] Fichier source: {source_file}")
        print("[TRACE] Transcript FTP:")
        for line in transcript:
            print(f"  {line}")
        return 0

    except Exception as exc:
        print(f"[FAIL] {exc}")
        print("[TRACE] Transcript FTP:")
        for line in transcript:
            print(f"  {line}")
        return 1
    finally:
        terminate_process(proc)
        if proc.stderr is not None:
            err = proc.stderr.read().decode("utf-8", errors="replace").strip()
            if err:
                print("[SERVER STDERR]")
                print(err)


if __name__ == "__main__":
    sys.exit(main())
