# Test configuration file

# Option to enable/disable tests
option(ENABLE_TESTS "Enable unit tests" ON)

if(ENABLE_TESTS)
    enable_testing()
    
    # Define test modules
    set(TEST_MODULES
        ConfigTest
        MathTest
        TimeTest
    )
    
    # List all available tests for users
    message(STATUS "Available test modules:")
    message(STATUS "  - ConfigTest : Tests Config class")
    message(STATUS "  - MathTest   : Tests Math utilities")
    message(STATUS "  - TimeTest   : Tests Time management")
    message(STATUS "")
    message(STATUS "Usage examples:")
    message(STATUS "  cmake -DENABLE_TESTS=ON ..             # Enable all tests")
    message(STATUS "  cmake -DENABLE_TESTS=OFF ..            # Disable all tests")
    message(STATUS "  cmake -DONLY_TEST=ConfigTest ..        # Only build ConfigTest")
    message(STATUS "")
    
    # Option to only build specific test
    set(ONLY_TEST "" CACHE STRING "Only build specified test (e.g., ConfigTest). Empty means build all enabled tests.")
    
    # Helper function to add a test
    function(add_engine_test test_name test_sources)
        if(ONLY_TEST STREQUAL "" OR ONLY_TEST STREQUAL test_name)
            add_executable(${test_name} ${test_sources})
            target_include_directories(${test_name} PRIVATE ${TARGET_INCLUDE_DIRS})
            target_link_libraries(${test_name} PRIVATE gtest gtest_main ${TARGET_LINK_LIBS})
            add_test(NAME ${test_name} COMMAND ${test_name})
            message(STATUS "✓ Added test: ${test_name}")
        endif()
    endfunction()
    
    # Add individual tests
    if(ONLY_TEST STREQUAL "" OR ONLY_TEST STREQUAL "ConfigTest")
        add_engine_test(ConfigTest "test/core/config_test.cpp")
    endif()
    
    if(ONLY_TEST STREQUAL "" OR ONLY_TEST STREQUAL "MathTest")
        add_engine_test(MathTest "test/utils/math_test.cpp")
    endif()
    
    if(ONLY_TEST STREQUAL "" OR ONLY_TEST STREQUAL "TimeTest")
        add_engine_test(TimeTest "test/core/time_test.cpp")
    endif()
    
    # Convenience target to run all tests
    add_custom_target(run_tests
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS ${TEST_MODULES}
        COMMENT "Running all tests..."
    )
    
endif()
