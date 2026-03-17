// Copyright Sylar129

#pragma once

#include <cstdlib>

#include "log.h"

#ifdef NDEBUG
#define DEBUG_BREAK() std::abort()
#else
#ifdef _MSC_VER
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() __builtin_trap()
#endif
#endif

// Assert macros - terminates program with critical log on failure
// Debug mode: enters breakpoint | Release mode: crashes
#define ENGINE_LOG_ASSERT(condition, message)                                  \
  {                                                                            \
    if (!(condition)) {                                                        \
      ENGINE_LOG_CRITICAL("Assertion failed: [{}] - {}", #condition, message); \
      DEBUG_BREAK();                                                           \
    }                                                                          \
  }

#define GAME_LOG_ASSERT(condition, message)                                  \
  {                                                                          \
    if (!(condition)) {                                                      \
      GAME_LOG_CRITICAL("Assertion failed: [{}] - {}", #condition, message); \
      DEBUG_BREAK();                                                         \
    }                                                                        \
  }
