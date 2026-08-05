# Build with MSVC

```powershell
cmake --preset windows-msvc
cmake --build --preset windows-msvc-build --config Release
ctest --preset windows-msvc-test -C Release
```
