#!/usr/bin/env python3
"""Live RFC959 PASV+STOR test against ./myftp.

Flow:
- start local myftp server
- USER/PASS login
- PASV
- open data socket to advertised endpoint
- STOR unique file name over data socket
- verify file exists in FTP root with expected bytes
"""

from __future__ import annotations

import argparse
import os
import random
import re
import socket
import subprocess
import sys
import threading
import time
import uuid
from pathlib import Path

PASV_RE = re.compile(r"\((\d+),(\d+),(\d+),(\d+),(\d+),(\d+)\)")
STOR_PAYLOAD_SIZE = 1 * 1024 * 1024  # 1 MiB
PARALLEL_STOR_COUNT = 2


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
    data = (command + "\r\n").encode("ascii")
    ctrl_sock.sendall(data)


def parse_pasv_endpoint(response_lines: list[str]) -> tuple[str, int]:
    joined = " ".join(response_lines)
    match = PASV_RE.search(joined)
    if not match:
        raise TestError(f"Impossible de parser PASV: {joined!r}")
    h1, h2, h3, h4, p1, p2 = (int(match.group(i)) for i in range(1, 7))
    if not all(0 <= x <= 255 for x in (h1, h2, h3, h4, p1, p2)):
        raise TestError(f"PASV contient des octets invalides: {joined!r}")
    host = f"{h1}.{h2}.{h3}.{h4}"
    port = p1 * 256 + p2
    return host, port


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


def make_payload(worker_id: int) -> bytes:
    header = (
        "RFC959 PASV STOR parallel test\n"
        f"worker={worker_id}\n"
        f"time={int(time.time())}\n"
        f"nonce={random.randint(100000, 999999)}\n"
    ).encode("utf-8")
    if len(header) > STOR_PAYLOAD_SIZE:
        raise TestError("En-tete plus grand que la taille de payload demandee")
    return header + (b"A" * (STOR_PAYLOAD_SIZE - len(header)))


def run_one_stor(control_port: int, filename: str, payload: bytes) -> list[str]:
    transcript: list[str] = []

    ctrl = socket.create_connection(("127.0.0.1", control_port), timeout=3.0)
    ctrl.settimeout(6.0)
    ctrl_file = ctrl.makefile("rb")

    try:
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

        data_sock = socket.create_connection((pasv_host, pasv_port), timeout=6.0)
        data_sock.settimeout(6.0)

        send_cmd(ctrl, f"STOR {filename}")
        transcript.append(f"C: STOR {filename}")
        code, lines = read_response(ctrl_file)
        transcript += [f"S: {ln}" for ln in lines]
        if code not in (125, 150):
            raise TestError(f"Code attendu 125/150 avant transfer STOR, recu {code}")

        data_sock.sendall(payload)
        data_sock.shutdown(socket.SHUT_WR)
        data_sock.close()

        code, lines = read_response(ctrl_file)
        transcript += [f"S: {ln}" for ln in lines]
        if code != 226:
            raise TestError(f"Code attendu 226 apres STOR, recu {code}")

        send_cmd(ctrl, "QUIT")
        transcript.append("C: QUIT")
        code, lines = read_response(ctrl_file)
        transcript += [f"S: {ln}" for ln in lines]
        if code != 221:
            raise TestError(f"Code attendu 221 apres QUIT, recu {code}")
        return transcript
    finally:
        ctrl_file.close()
        ctrl.close()


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Test live RFC959: PASV + STOR contre ./myftp"
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
    filenames = [
        f"upload_parallel_{i}_{uuid.uuid4().hex[:10]}.txt"
        for i in range(PARALLEL_STOR_COUNT)
    ]
    target_files = [root_dir / name for name in filenames]
    payloads = [make_payload(i) for i in range(PARALLEL_STOR_COUNT)]
    for path in target_files:
        if path.exists():
            print(f"[FAIL] Le fichier cible existe deja: {path}")
            return 2

    proc = subprocess.Popen(
        [str(server_bin), str(control_port), str(root_dir)],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        preexec_fn=os.setsid,
    )

    transcript_lock = threading.Lock()
    transcript: list[str] = []
    worker_errors: list[str | None] = [None] * PARALLEL_STOR_COUNT
    worker_transcripts: list[list[str]] = [[] for _ in range(PARALLEL_STOR_COUNT)]

    try:
        wait_server_ready(control_port)

        def worker(i: int) -> None:
            try:
                local_transcript = run_one_stor(control_port, filenames[i], payloads[i])
                worker_transcripts[i] = local_transcript
            except Exception as exc:
                worker_errors[i] = str(exc)
            finally:
                with transcript_lock:
                    prefix = f"[W{i}] "
                    for line in worker_transcripts[i]:
                        transcript.append(prefix + line)
                    if worker_errors[i]:
                        transcript.append(prefix + "ERROR: " + worker_errors[i])

        threads = [
            threading.Thread(target=worker, args=(i,))
            for i in range(PARALLEL_STOR_COUNT)
        ]
        for th in threads:
            th.start()
        for th in threads:
            th.join()

        for i, err in enumerate(worker_errors):
            if err is not None:
                raise TestError(f"Worker {i} en echec: {err}")
        for i, target_file in enumerate(target_files):
            if not target_file.exists():
                raise TestError(f"Fichier non cree: {target_file}")
            received = target_file.read_bytes()
            if received != payloads[i]:
                raise TestError(
                    "Contenu incorrect worker "
                    f"{i} (attendu {len(payloads[i])} octets, recu {len(received)})"
                )

        print("[PASS] Test RFC959 PASV+STOR x2 valide")
        print(f"[INFO] Serveur: {server_bin}")
        print(f"[INFO] Racine FTP: {root_dir}")
        print(f"[INFO] Port controle: {control_port}")
        print(f"[INFO] Uploads paralleles: {PARALLEL_STOR_COUNT}")
        print(f"[INFO] Taille payload STOR: {STOR_PAYLOAD_SIZE} octets")
        for path in target_files:
            print(f"[INFO] Fichier stocke: {path}")
        print("[TRACE] Transcript FTP:")
        for line in transcript:
            print(f"  {line}")
        return 0

    except Exception as exc:
        print(f"[FAIL] {exc}")
        print("[TRACE] Transcript FTP:")
        for line in transcript:
            print(f"  {line}")
        try:
            for target_file in target_files:
                if target_file.exists():
                    print(f"[INFO] Fichier cree quand meme: {target_file}")
        except OSError:
            pass
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
