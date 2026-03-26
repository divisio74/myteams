#!/usr/bin/env python3
"""Repro crash LIST against an already running FTP server.

Usage example:
  python3 tests/rfc959_pasv_list_crash_repro.py --host 127.0.0.1 --port 2121
"""

from __future__ import annotations

import argparse
from concurrent.futures import ThreadPoolExecutor, as_completed
import re
import socket
import sys
import time

PASV_RE = re.compile(r"\((\d+),(\d+),(\d+),(\d+),(\d+),(\d+)\)")


class ReproError(RuntimeError):
    pass


def read_response(ctrl_file) -> tuple[int, list[str]]:
    line = ctrl_file.readline()
    if not line:
        raise ReproError("Connexion controle fermee")
    line = line.decode("utf-8", errors="replace")
    lines = [line.rstrip("\r\n")]
    if len(line) < 3 or not line[:3].isdigit():
        raise ReproError(f"Reponse FTP invalide: {line!r}")
    code = int(line[:3])

    if len(line) >= 4 and line[3] == "-":
        end_prefix = f"{code} "
        while True:
            next_line = ctrl_file.readline()
            if not next_line:
                raise ReproError("Fin de flux sur reponse multilignes")
            decoded = next_line.decode("utf-8", errors="replace").rstrip("\r\n")
            lines.append(decoded)
            if decoded.startswith(end_prefix):
                break
    return code, lines


def send_cmd(ctrl_sock: socket.socket, command: str) -> None:
    ctrl_sock.sendall((command + "\r\n").encode("ascii", errors="ignore"))


def parse_pasv(lines: list[str]) -> tuple[str, int]:
    joined = " ".join(lines)
    match = PASV_RE.search(joined)
    if not match:
        raise ReproError(f"Impossible de parser PASV: {joined!r}")
    h1, h2, h3, h4, p1, p2 = (int(match.group(i)) for i in range(1, 7))
    return f"{h1}.{h2}.{h3}.{h4}", p1 * 256 + p2


def run_once(host: str, port: int, list_arg: str, timeout: float) -> None:
    ctrl = socket.create_connection((host, port), timeout=timeout)
    ctrl.settimeout(timeout)
    ctrl_file = ctrl.makefile("rb")

    try:
        code, _ = read_response(ctrl_file)
        if code != 220:
            raise ReproError(f"Banner inattendu: {code}")

        send_cmd(ctrl, "USER Anonymous")
        code, _ = read_response(ctrl_file)
        if code != 331:
            raise ReproError(f"USER inattendu: {code}")

        send_cmd(ctrl, "PASS")
        code, _ = read_response(ctrl_file)
        if code != 230:
            raise ReproError(f"PASS inattendu: {code}")

        send_cmd(ctrl, "PASV")
        code, pasv_lines = read_response(ctrl_file)
        if code != 227:
            raise ReproError(f"PASV inattendu: {code}")
        data_host, data_port = parse_pasv(pasv_lines)

        data_sock = socket.create_connection((data_host, data_port), timeout=timeout)
        data_sock.settimeout(timeout)

        send_cmd(ctrl, f"LIST {list_arg}")
        read_response(ctrl_file)
        try:
            while data_sock.recv(8192):
                pass
        except OSError:
            pass
        data_sock.close()
        try:
            read_response(ctrl_file)
        except ReproError:
            pass

        send_cmd(ctrl, "QUIT")
        try:
            read_response(ctrl_file)
        except ReproError:
            pass
    finally:
        ctrl_file.close()
        ctrl.close()


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Stress LIST pour reproduire un crash (serveur deja lance)"
    )
    parser.add_argument("--host", default="127.0.0.1", help="Host FTP control")
    parser.add_argument("--port", type=int, required=True, help="Port FTP control")
    parser.add_argument("--iterations", type=int, default=80, help="Nombre de passes")
    parser.add_argument("--parallel", type=int, default=10, help="Nombre de LIST en parallele")
    parser.add_argument("--token-len", type=int, default=500, help="Longueur de chaque token")
    parser.add_argument("--tokens", type=int, default=80, help="Nombre de tokens dans LIST")
    parser.add_argument("--timeout", type=float, default=2.0, help="Timeout socket")
    args = parser.parse_args()

    list_arg = " ".join(["A" * args.token_len for _ in range(args.tokens)])
    print(
        f"[INFO] Repro LIST: host={args.host} port={args.port} "
        f"iterations={args.iterations} parallel={args.parallel} "
        f"token_len={args.token_len} tokens={args.tokens}"
    )

    for i in range(args.iterations):
        try:
            with ThreadPoolExecutor(max_workers=args.parallel) as executor:
                futures = [
                    executor.submit(run_once, args.host, args.port, list_arg, args.timeout)
                    for _ in range(args.parallel)
                ]
                for future in as_completed(futures):
                    future.result()
            print(
                f"[INFO] iteration {i + 1}/{args.iterations}: "
                f"{args.parallel} LIST paralleles ok"
            )
        except Exception as exc:
            print(f"[CRASH?] iteration {i + 1}: {exc}")
            time.sleep(0.1)
            try:
                with socket.create_connection((args.host, args.port), timeout=args.timeout):
                    pass
                print("[INFO] serveur encore joignable")
            except OSError:
                print("[HIT] serveur non joignable apres l'erreur (crash probable)")
            return 1

    print("[INFO] Aucun crash detecte avec ces parametres")
    return 0


if __name__ == "__main__":
    sys.exit(main())
