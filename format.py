#!/usr/bin/env python3
"""Format C++ and CMake source files for the SunnyLand project.

Runs clang-format on all C++ sources under src/ and cmake-format on all
CMake files in the project (excluding build/ and third_party/).  Both
tools are installed automatically via pip when not found on PATH.

Usage:
    python format.py           Format files in-place.
    python format.py --check   Check formatting only; exit 1 if any file
                               needs changes.
"""

from __future__ import annotations

import argparse
import shutil
import subprocess
import sys
from pathlib import Path

ROOT: Path = Path(__file__).resolve().parent

_CPP_EXTENSIONS: frozenset[str] = frozenset({".cpp", ".h", ".hpp", ".cc", ".cxx"})
_SKIP_DIRS: frozenset[Path] = frozenset({ROOT / "build", ROOT / "third_party"})


def _pip_install(package: str) -> None:
    """Install a Python package using the current interpreter's pip.

    Args:
        package: The PyPI package name to install.

    Raises:
        subprocess.CalledProcessError: If pip exits with a non-zero status.
    """
    print(f"  Installing {package} via pip...")
    subprocess.run(
        [sys.executable, "-m", "pip", "install", "--quiet", package],
        check=True,
    )


def _ensure_clang_format() -> str:
    """Return the clang-format executable path, installing it if missing.

    Tries PATH first.  If not found, installs the ``clang-format`` pip
    package and resolves the bundled binary from the package data directory.

    Returns:
        Absolute path to the clang-format executable.

    Raises:
        SystemExit: If the tool cannot be located even after installation.
    """
    exe = shutil.which("clang-format")
    if exe:
        return exe

    print("clang-format not found — installing via pip...")
    _pip_install("clang-format")

    exe = shutil.which("clang-format")
    if exe:
        return exe

    # pip may install scripts outside PATH; probe the package data directory.
    result = subprocess.run(
        [sys.executable, "-c",
         "import clang_format; print(clang_format.__file__)"],
        capture_output=True,
        text=True,
    )
    if result.returncode == 0:
        bin_dir = Path(result.stdout.strip()).parent / "data" / "bin"
        candidates = list(bin_dir.glob("clang-format*"))
        if candidates:
            return str(candidates[0])

    sys.exit(
        "ERROR: clang-format could not be located after installation.\n"
        "Install LLVM manually: https://releases.llvm.org/"
    )


def _ensure_cmake_format() -> str:
    """Return the cmake-format executable path, installing it if missing.

    Returns:
        Absolute path to the cmake-format executable.

    Raises:
        SystemExit: If the tool cannot be located even after installation.
    """
    exe = shutil.which("cmake-format")
    if exe:
        return exe

    print("cmake-format not found — installing via pip...")
    _pip_install("cmake-format")

    exe = shutil.which("cmake-format")
    if exe:
        return exe

    sys.exit(
        "ERROR: cmake-format could not be located after installation.\n"
        "Try manually: pip install cmake-format"
    )


def collect_cpp_files() -> list[Path]:
    """Return sorted list of C++ source files under src/.

    Returns:
        Sorted list of Path objects for all C++ files found.
    """
    return sorted(
        p for p in ROOT.joinpath("src").rglob("*")
        if p.suffix in _CPP_EXTENSIONS
    )


def collect_cmake_files() -> list[Path]:
    """Return sorted list of CMake files, excluding build/ and third_party/.

    Returns:
        Sorted list of Path objects for all CMakeLists.txt and *.cmake files.
    """
    files = []
    for path in ROOT.rglob("*"):
        if any(path.is_relative_to(skip) for skip in _SKIP_DIRS):
            continue
        if path.name == "CMakeLists.txt" or path.suffix == ".cmake":
            files.append(path)
    return sorted(files)


def run_clang_format(
    files: list[Path],
    clang_fmt: str,
    check_only: bool,
) -> bool:
    """Run clang-format on a list of C++ files.

    Args:
        files: List of paths to format.
        clang_fmt: Path to the clang-format executable.
        check_only: When True, report violations without modifying files.

    Returns:
        True if all files are properly formatted, False otherwise.
    """
    print(f"=== clang-format ({len(files)} files) ===")
    all_clean = True
    for path in files:
        rel = path.relative_to(ROOT)
        if check_only:
            result = subprocess.run(
                [clang_fmt, "--dry-run", "--Werror", "-style=file", str(path)],
                capture_output=True,
            )
            if result.returncode != 0:
                print(f"  [needs format] {rel}")
                all_clean = False
        else:
            subprocess.run(
                [clang_fmt, "-i", "-style=file", str(path)],
                check=True,
            )
            print(f"  [formatted]    {rel}")
    return all_clean


def run_cmake_format(
    files: list[Path],
    cmake_fmt: str,
    check_only: bool,
) -> bool:
    """Run cmake-format on a list of CMake files.

    Args:
        files: List of paths to format.
        cmake_fmt: Path to the cmake-format executable.
        check_only: When True, report violations without modifying files.

    Returns:
        True if all files are properly formatted, False otherwise.
    """
    print(f"\n=== cmake-format ({len(files)} files) ===")
    all_clean = True
    for path in files:
        rel = path.relative_to(ROOT)
        if check_only:
            result = subprocess.run(
                [cmake_fmt, "--check", str(path)],
                capture_output=True,
            )
            if result.returncode != 0:
                print(f"  [needs format] {rel}")
                all_clean = False
        else:
            subprocess.run([cmake_fmt, "-i", str(path)], check=True)
            print(f"  [formatted]    {rel}")
    return all_clean


def main() -> None:
    """Parse arguments, ensure tools are available, and run formatters."""
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "--check",
        action="store_true",
        help="Check formatting only; exit 1 if any file needs changes.",
    )
    args = parser.parse_args()

    print("Checking / installing required tools...")
    clang_fmt = _ensure_clang_format()
    cmake_fmt = _ensure_cmake_format()
    print(f"  clang-format : {clang_fmt}")
    print(f"  cmake-format : {cmake_fmt}")
    print()

    cpp_ok = run_clang_format(collect_cpp_files(), clang_fmt, args.check)
    cmake_ok = run_cmake_format(collect_cmake_files(), cmake_fmt, args.check)

    print()
    if args.check:
        if cpp_ok and cmake_ok:
            print("Check PASSED — all files are properly formatted.")
        else:
            print(
                "Check FAILED — some files need formatting.\n"
                "Run: python format.py"
            )
            sys.exit(1)
    else:
        print("Done.")


if __name__ == "__main__":
    main()

