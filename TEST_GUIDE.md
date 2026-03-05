# SunnyLand Test Framework Guide

This document explains how to build and run unit tests for the SunnyLand engine.

## Overview

The project uses **Google Test (gtest)** for unit testing. The test framework is integrated into CMake and allows you to:
- Build and run all tests
- Build and run specific test modules
- Disable tests completely

## Available Test Modules

### 1. ConfigTest
Tests the `Config` class functionality:
- Default value initialization
- Loading from and saving to JSON files
- FPS validation
- Window configuration
- Graphics settings
- Input mappings

**File:** `test/core/config_test.cpp`

### 2. MathTest
Tests mathematical utilities:
- Vector2 operations (construction, addition, subtraction, multiplication)
- Vector magnitude and normalization
- Dot product and distance calculations
- Angle conversions (radians ↔ degrees)

**File:** `test/utils/math_test.cpp`

### 3. TimeTest
Tests time management functionality:
- Frame timing and delta time calculations
- Elapsed time tracking
- Frame counting
- Reset functionality
- Time consistency verification

**File:** `test/core/time_test.cpp`

## Building and Running Tests

### 1. Build Everything with Tests (Default)

```bash
cd build
cmake .. -DENABLE_TESTS=ON
make
```

### 2. Build and Run All Tests

After building, run all tests with:

```bash
ctest --output-on-failure
```

Or use the convenience target:

```bash
make run_tests
```

### 3. Build Only a Specific Test Module

```bash
cmake .. -DENABLE_TESTS=ON -DONLY_TEST=ConfigTest
make
ctest --output-on-failure
```

Available test modules:
- `ConfigTest` - Config class tests
- `MathTest` - Math utilities tests
- `TimeTest` - Time management tests

### 4. Build Without Tests

```bash
cmake .. -DENABLE_TESTS=OFF
make
```

### 5. Run a Specific Test

```bash
./ConfigTest        # Run ConfigTest
./MathTest         # Run MathTest
./TimeTest         # Run TimeTest
```

### 6. Verbose Test Output

For more detailed test output:

```bash
ctest --output-on-failure -V
```

## Adding New Tests

### Step 1: Create Test File

Create a new test file in the appropriate directory:
- `test/core/` for core module tests
- `test/utils/` for utility module tests
- `test/render/` for render module tests (create if needed)
- etc.

### Step 2: Write Test Code

```cpp
#include <gtest/gtest.h>
#include "engine/module/header.h"

class ModuleTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Setup code
  }

  void TearDown() override {
    // Cleanup code
  }
};

TEST_F(ModuleTest, TestName) {
  // Test implementation
  EXPECT_EQ(actual, expected);
}
```

### Step 3: Register Test in cmake/Tests.cmake

Add to the helper function section:

```cmake
if(ONLY_TEST STREQUAL "" OR ONLY_TEST STREQUAL "ModuleTest")
    add_engine_test(ModuleTest "test/module/module_test.cpp")
endif()
```

Also add to the `TEST_MODULES` list and the status message.

### Step 4: Rebuild and Test

```bash
cmake .. -DENABLE_TESTS=ON
make
ctest --output-on-failure
```

## Common Test Assertions

### Equality Assertions
```cpp
EXPECT_EQ(actual, expected);      // Non-fatal assertion
ASSERT_EQ(actual, expected);      // Fatal assertion
EXPECT_NE(actual, unexpected);    // Not equal
```

### Floating Point Assertions
```cpp
EXPECT_FLOAT_EQ(actual, expected);     // Exact match
EXPECT_NEAR(actual, expected, 0.001);  // Within tolerance
```

### Boolean Assertions
```cpp
EXPECT_TRUE(condition);
EXPECT_FALSE(condition);
ASSERT_TRUE(condition);
ASSERT_FALSE(condition);
```

### Comparison Assertions
```cpp
EXPECT_LT(a, b);    // Less than
EXPECT_LE(a, b);    // Less than or equal
EXPECT_GT(a, b);    // Greater than
EXPECT_GE(a, b);    // Greater than or equal
```

## CMake Integration Details

### Test Configuration Files
- `cmake/Tests.cmake` - Main test configuration
- `cmake/Dependencies.cmake` - Includes Google Test setup

### Key CMake Variables
- `ENABLE_TESTS` - Enable/disable test building (default: ON)
- `ONLY_TEST` - Build only a specific test module (default: "" = all)

### Key CMake Functions
```cmake
add_engine_test(test_name source_files)
```
Helper function that:
- Creates an executable for the test
- Links necessary libraries
- Registers the test with CTest
- Prints status messages

## Troubleshooting

### Google Test Failed to Download
If Google Test download fails, ensure you have internet connectivity and git installed. The framework will be downloaded from:
```
https://github.com/google/googletest.git (v1.14.0)
```

### Test Linking Errors
Ensure all required libraries are linked in `cmake/LinkLibraries.cmake`

### Test File Not Found
Verify the test file path in `cmake/Tests.cmake` is correct and relative to the project root.

### Assertion Failures
Run with verbose output to get more details:
```bash
ctest --output-on-failure -V
./TestName                    # Run individual tests
ctest -R "TestName" -V        # Filter tests by name
```

## Continuous Integration

For CI/CD pipelines, you can:

```bash
cmake .. -DENABLE_TESTS=ON
make
ctest --output-on-failure --timeout 30
```

The `--timeout 30` flag sets a 30-second timeout per test.

## Performance Notes

- Tests are independent and can run in parallel
- Each test has setup/teardown phases for isolation
- File I/O tests clean up temporary files automatically
- Time tests use sleep which may cause slight variance

## Future Enhancements

Potential areas for test expansion:
- [ ] Input manager tests
- [ ] Renderer tests
- [ ] Resource manager tests
- [ ] Game object tests
- [ ] Component system tests
- [ ] Integration tests
- [ ] Performance benchmarks with Google Benchmark
