import argparse
import re

from pathlib import Path


def read_file(file_path) -> Path | None:
    try:
        return Path(file_path).read_text(encoding="utf-8")
    except FileNotFoundError:
        print(f"File '{file_path}' not found.")

    return None


MATCH_STRING = r"(love\.\w+\.\w+)\(\)\s+==>\s+\w+\s+-\s+(\d+\.\d+)"
TIME_REGEX = re.compile(MATCH_STRING, re.MULTILINE)


def main() -> None:
    parser = argparse.ArgumentParser(description="Read a file.")
    parser.add_argument("file", type=str, help="Path to the file")
    parser.add_argument(
        "--top", "-t", action="store_true", help="Get top 3 slowest methods"
    )

    args = parser.parse_args()

    content = read_file(args.file)

    if not content:
        print("No content found in the file.")
        return

    times = re.findall(TIME_REGEX, content)

    if not times:
        print("No times found in the file.")
        return

    sorted_times = sorted(times, key=lambda x: float(x[1][:-1]), reverse=True)

    if args.top:
        for time in sorted_times[:3]:
            method, total_time = time
            print(f"- {method}: {total_time}s")

        return

    for time in sorted_times:
        method, total_time = time
        print(f"- {method}: {total_time}s")


if __name__ == "__main__":
    main()
