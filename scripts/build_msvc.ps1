$ErrorActionPreference = "Stop"
cmake --preset msvc-release
cmake --build --preset msvc-release
ctest --preset msvc-release
