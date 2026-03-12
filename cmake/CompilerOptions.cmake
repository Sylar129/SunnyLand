# Compiler and compiler options configuration

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

function(apply_project_compiler_options target_name)
  target_compile_definitions(${target_name} PRIVATE JSON_NOEXCEPTION
                                                   SPDLOG_NO_EXCEPTIONS)

  if (MSVC)
    target_compile_options(${target_name} PRIVATE /W4)
    target_compile_definitions(${target_name} PRIVATE _HAS_EXCEPTIONS=0)
  else()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic
                                                  -fno-exceptions)
  endif()
endfunction()

if (NOT MSVC)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()
