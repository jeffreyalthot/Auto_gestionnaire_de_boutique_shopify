# Build with MSYS2 MinGW64

```bash
cmake --preset windows-msys2-mingw64
cmake --build --preset windows-msys2-mingw64-build --parallel 2
ctest --preset windows-msys2-mingw64-test
```
