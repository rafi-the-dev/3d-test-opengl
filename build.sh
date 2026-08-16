#!/usr/bin/env bash
#
# build.sh - Compile this project for Linux and mark its binary executable.
#
# Usage:
#   ./build.sh            # build (Release)
#   ./build.sh -c         # clean rebuild
#   ./build.sh -t Debug   # debug build
#
set -uo pipefail

# Run relative to this script's own directory so it works from anywhere.
SRC_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SRC_DIR"

BUILD_TYPE="Release"
CLEAN=0
while [[ $# -gt 0 ]]; do
  case "$1" in
    -t|--type)  BUILD_TYPE="$2"; shift 2 ;;
    -c|--clean) CLEAN=1; shift ;;
    -h|--help)  grep '^#' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
    *) echo "Unknown option: $1"; exit 1 ;;
  esac
done

# Prefer Ninja when available (faster, parallel); fall back to Make.
if command -v ninja >/dev/null 2>&1; then
  GENERATOR="Ninja"
else
  GENERATOR="Unix Makefiles"
fi
JOBS="$(nproc 2>/dev/null || echo 4)"

BUILD_DIR="$SRC_DIR/build"
[[ $CLEAN -eq 1 ]] && rm -rf "$BUILD_DIR"

echo ">>> Configuring '$(basename "$SRC_DIR")' [$BUILD_TYPE, $GENERATOR]"
# CMAKE_EXPORT_COMPILE_COMMANDS makes CMake write build/compile_commands.json,
# which editors (VS Code C/C++, clangd, CLion) read to resolve include paths
# (imgui, glfw, gl2d, ...) and to know the real Linux compile flags/defines.
cmake -S "$SRC_DIR" -B "$BUILD_DIR" -G "$GENERATOR" \
      -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON || exit 1

# Expose the compile database at the project root so the editor finds it.
if [[ -f "$BUILD_DIR/compile_commands.json" ]]; then
  ln -sf "build/compile_commands.json" "$SRC_DIR/compile_commands.json"
  echo ">>> compile_commands.json -> $SRC_DIR/compile_commands.json"
fi

echo ">>> Building with $JOBS jobs"
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" -j "$JOBS" || exit 1

# Mark every freshly built ELF executable as runnable.
while IFS= read -r -d '' bin; do
  chmod +x "$bin"
  echo ">>> executable: $bin"
done < <(find "$BUILD_DIR" -maxdepth 2 -type f -perm -u+x \
           -exec sh -c 'file -b "$1" | grep -q ELF' _ {} \; -print0 2>/dev/null)

echo ">>> Done."
