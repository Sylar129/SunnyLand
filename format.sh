#!/usr/bin/env bash
# AI-generated code.
# format.sh — format C++ sources with clang-format and CMake files with cmake-format
#
# Usage:
#   ./format.sh           — format files in-place
#   ./format.sh --check   — check formatting without modifying files (exit 1 if any file needs changes)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CHECK_ONLY=0

# ---------------------------------------------------------------------------
# Parse arguments
# ---------------------------------------------------------------------------
for arg in "$@"; do
  case "$arg" in
    --check) CHECK_ONLY=1 ;;
    *) echo "Unknown argument: $arg" >&2; exit 1 ;;
  esac
done

# ---------------------------------------------------------------------------
# Tool detection
# ---------------------------------------------------------------------------
if ! command -v clang-format &>/dev/null; then
  echo "ERROR: clang-format not found. Install it with: brew install clang-format" >&2
  exit 1
fi

if ! command -v cmake-format &>/dev/null; then
  echo "ERROR: cmake-format not found. Install it with: pip install cmake-format" >&2
  exit 1
fi

# ---------------------------------------------------------------------------
# Collect files
# ---------------------------------------------------------------------------
CPP_FILES=()
while IFS= read -r -d '' f; do
  CPP_FILES+=("$f")
done < <(find "$SCRIPT_DIR/src" \( -name '*.cpp' -o -name '*.h' \) -print0)

CMAKE_FILES=()
while IFS= read -r -d '' f; do
  CMAKE_FILES+=("$f")
done < <(find "$SCRIPT_DIR" \
  -not \( -path "$SCRIPT_DIR/build/*" -prune \) \
  -not \( -path "$SCRIPT_DIR/third_party/*" -prune \) \
  \( -name 'CMakeLists.txt' -o -name '*.cmake' \) -print0)

# ---------------------------------------------------------------------------
# Run formatters
# ---------------------------------------------------------------------------
FAILED=0

echo "=== clang-format (${#CPP_FILES[@]} files) ==="
for f in "${CPP_FILES[@]}"; do
  rel="${f#"$SCRIPT_DIR/"}"
  if [[ $CHECK_ONLY -eq 1 ]]; then
    if ! clang-format --dry-run --Werror -style=file "$f" &>/dev/null; then
      echo "  [needs format] $rel"
      FAILED=1
    fi
  else
    clang-format -i -style=file "$f"
    echo "  [formatted]    $rel"
  fi
done

echo ""
echo "=== cmake-format (${#CMAKE_FILES[@]} files) ==="
for f in "${CMAKE_FILES[@]}"; do
  rel="${f#"$SCRIPT_DIR/"}"
  if [[ $CHECK_ONLY -eq 1 ]]; then
    if ! cmake-format --check "$f" &>/dev/null; then
      echo "  [needs format] $rel"
      FAILED=1
    fi
  else
    cmake-format -i "$f"
    echo "  [formatted]    $rel"
  fi
done

# ---------------------------------------------------------------------------
# Summary
# ---------------------------------------------------------------------------
echo ""
if [[ $CHECK_ONLY -eq 1 ]]; then
  if [[ $FAILED -eq 1 ]]; then
    echo "Check FAILED — some files need formatting. Run ./format.sh to fix them."
    exit 1
  else
    echo "Check PASSED — all files are properly formatted."
  fi
else
  echo "Done."
fi
