## Quick Test Commands Reference

### Build and Test All Modules
```bash
cd build
cmake .. -DENABLE_TESTS=ON
make
ctest --output-on-failure
```

### Build Only ConfigTest
```bash
cmake .. -DENABLE_TESTS=ON -DONLY_TEST=ConfigTest
make
./ConfigTest
```

### Build Only MathTest
```bash
cmake .. -DENABLE_TESTS=ON -DONLY_TEST=MathTest
make
./MathTest
```

### Build Only TimeTest
```bash
cmake .. -DENABLE_TESTS=ON -DONLY_TEST=TimeTest
make
./TimeTest
```

### Build Without Tests
```bash
cmake .. -DENABLE_TESTS=OFF
make
```

### Run Tests After Building
```bash
ctest                              # Run with minimal output
ctest --output-on-failure          # Show failures
ctest --output-on-failure -V       # Verbose output
make run_tests                     # Using convenience target
```

### Run Specific Test Directly
```bash
./ConfigTest
./MathTest
./TimeTest
```

### View Available Tests
```bash
ctest --show-only
```

### Run Tests with Specific Filter
```bash
ctest -R "Config"          # Run tests matching "Config"
ctest -R "Vector"          # Run tests matching "Vector"
ctest -E "Large"           # Exclude tests matching "Large"
```

### Debug a Specific Test
```bash
ctest -R "ConfigTest" -VV  # Max verbosity for ConfigTest
gdb ./ConfigTest           # Run in debugger
```
