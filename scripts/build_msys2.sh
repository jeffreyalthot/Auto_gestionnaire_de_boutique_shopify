#!/usr/bin/env bash
set -euo pipefail
cmake --preset msys2-mingw64-release
cmake --build --preset msys2-mingw64-release
ctest --preset msys2-mingw64-release
