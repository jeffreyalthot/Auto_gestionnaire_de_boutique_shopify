#!/usr/bin/env bash
set -euo pipefail
cmake --preset linux-release
cmake --build --preset linux-release
ctest --preset linux-release
