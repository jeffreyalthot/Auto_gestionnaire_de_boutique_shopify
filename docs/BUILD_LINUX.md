# Build on Linux

```bash
cmake -S . -B build/linux -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/linux --parallel 2
ctest --test-dir build/linux --output-on-failure
```
