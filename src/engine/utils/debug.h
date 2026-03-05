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
#define ENGINE_ASSERT(condition, message)                               \
  do {                                                                  \
    if (!(condition)) {                                                 \
      ::Log::GetEngineLogger()->critical("Assertion failed: [{}] - {}", \
                                         #condition, message);          \
      DEBUG_BREAK();                                                    \
    }                                                                   \
  } while (0)

#define GAME_ASSERT(condition, message)                               \
  do {                                                                \
    if (!(condition)) {                                               \
      ::Log::GetGameLogger()->critical("Assertion failed: [{}] - {}", \
                                       #condition, message);          \
      DEBUG_BREAK();                                                   \
    }                                                                 \
  } while (0)
