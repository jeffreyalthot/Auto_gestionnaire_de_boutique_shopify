# Build with MSYS2 MinGW64

Open the **MSYS2 MinGW x64** shell (not the plain `MSYS` shell), then install the native
MinGW dependencies:

```bash
pacman -S --needed \
  mingw-w64-x86_64-cmake \
  mingw-w64-x86_64-ninja \
  mingw-w64-x86_64-gcc \
  mingw-w64-x86_64-pkgconf \
  mingw-w64-x86_64-curl \
  mingw-w64-x86_64-json-c \
  mingw-w64-x86_64-openssl \
  mingw-w64-x86_64-sqlite3
```

Verify that the MinGW toolchain is first on `PATH`:

```bash
which cmake ninja g++ pkg-config
pkg-config --modversion libcurl json-c
```

Expected paths begin with `/mingw64/bin/`. If they begin with `/usr/bin/`, reopen the
**MSYS2 MinGW x64** shell or move `/mingw64/bin` before `/usr/bin` on `PATH`.

Configure, build, and test:

```bash
cmake --preset windows-msys2-mingw64
cmake --build --preset windows-msys2-mingw64-build --parallel 2
ctest --preset windows-msys2-mingw64-test
```

The `windows-msys2-mingw64` preset uses `cmake/MingwToolchain.cmake`, which pins CMake to
the MinGW-w64 compiler family and adds `$MINGW_PREFIX` (normally `/mingw64`) to CMake and
`pkg-config` search paths. This prevents accidental configuration with the POSIX MSYS
compiler or host libraries.
