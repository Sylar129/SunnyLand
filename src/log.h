// Copyright Sylar129

#pragma once

#include <memory>

#include "spdlog/logger.h"

class Log {
 public:
  static void Init();

  inline static std::shared_ptr<spdlog::logger>& GetEngineLogger() {
    return s_EngineLogger;
  }

  inline static std::shared_ptr<spdlog::logger>& GetGameLogger() {
    return s_GameLogger;
  }

 private:
  static std::shared_ptr<spdlog::logger> s_EngineLogger;
  static std::shared_ptr<spdlog::logger> s_GameLogger;
};

#define ENGINE_TRACE(...) ::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define ENGINE_DEBUG(...) ::Log::GetEngineLogger()->debug(__VA_ARGS__)
#define ENGINE_INFO(...) ::Log::GetEngineLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...) ::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...) ::Log::GetEngineLogger()->error(__VA_ARGS__)
#define ENGINE_CRITICAL(...) ::Log::GetEngineLogger()->critical(__VA_ARGS__)

#define GAME_TRACE(...) ::Log::GetGameLogger()->trace(__VA_ARGS__)
#define GAME_DEBUG(...) ::Log::GetGameLogger()->debug(__VA_ARGS__)
#define GAME_INFO(...) ::Log::GetGameLogger()->info(__VA_ARGS__)
#define GAME_WARN(...) ::Log::GetGameLogger()->warn(__VA_ARGS__)
#define GAME_ERROR(...) ::Log::GetGameLogger()->error(__VA_ARGS__)
#define GAME_CRITICAL(...) ::Log::GetGameLogger()->critical(__VA_ARGS__)
