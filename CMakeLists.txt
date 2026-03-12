cmake_minimum_required(VERSION 3.21.0)
project(SunnyLand VERSION 0.0.1 LANGUAGES CXX)

set(TARGET ${PROJECT_NAME})

# ============================================================================
# Configuration
# ============================================================================
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerOptions.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake)

# ============================================================================
# Sources and includes
# ============================================================================
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Sources.cmake)
include_directories(${TARGET_INCLUDE_DIRS})

# ============================================================================
# Target executable
# ============================================================================
add_executable(${TARGET} ${TARGET_SOURCES})

# ============================================================================
# Link libraries
# ============================================================================
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/LinkLibraries.cmake)
target_link_libraries(${TARGET} PRIVATE ${TARGET_LINK_LIBS})

# ============================================================================
# Post-build steps
# ============================================================================
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PostBuildSteps.cmake)