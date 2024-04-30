import sys
import argparse
import time
import os

from io import TextIOWrapper
from socket import gaierror, socket, AF_INET, SOCK_STREAM
from pathlib import Path


def clear_console() -> None:
    match os.name:
        case "nt":
            os.system("cls")
        case _:
            os.system("clear")


def main() -> None:
    parser = argparse.ArgumentParser()

    parser.add_argument("host", help="the host to connect to")
    parser.add_argument(
        "-l", "--log", action="store_true", help="write data to log file"
    )

    args = parser.parse_args()

    clear_console()

    tcp_socket = socket(AF_INET, SOCK_STREAM, 0)

    try:
        tcp_socket.connect((args.host, 8000))
        # tcp_socket.settimeout(3)
    except (ConnectionRefusedError, TimeoutError):
        print(f"Failed to connect to {args.host}:8000")
        sys.exit(1)
    except gaierror:
        print(f"Invalid host: {args.host}")
        sys.exit(1)

    log_file: TextIOWrapper = None

    if args.log:
        Path("logs").mkdir(exist_ok=True)
        timestamp = str(int(time.time()))
        log_file = open(f"logs/{timestamp}.txt", "w")

    while True:
        try:
            raw_data = tcp_socket.recv(0x400)

            if not len(raw_data):
                break

            data = raw_data.decode("utf-8")

            if args.log:
                log_file.write(data)

            print(data, end="")
        except ConnectionResetError:
            print("Connection reset by peer")
            break
        except KeyboardInterrupt:
            break
        except TimeoutError:
            print("Connection timed out")
            break

    tcp_socket.close()

    if args.log:
        log_file.close()


if __name__ == "__main__":
    main()
