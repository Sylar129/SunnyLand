// Copyright Sylar129

#pragma once

#include <memory>

#include "spdlog/logger.h"

class Log {
 public:
  static void Init();

  static std::shared_ptr<spdlog::logger>& GetEngineLogger() {
    return engine_logger_;
  }

  static std::shared_ptr<spdlog::logger>& GetGameLogger() {
    return game_logger_;
  }

 private:
  static std::shared_ptr<spdlog::logger> engine_logger_;
  static std::shared_ptr<spdlog::logger> game_logger_;
};

#define ENGINE_LOG_TRACE(...) ::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define ENGINE_LOG_DEBUG(...) ::Log::GetEngineLogger()->debug(__VA_ARGS__)
#define ENGINE_LOG_INFO(...) ::Log::GetEngineLogger()->info(__VA_ARGS__)
#define ENGINE_LOG_WARN(...) ::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define ENGINE_LOG_ERROR(...) ::Log::GetEngineLogger()->error(__VA_ARGS__)
#define ENGINE_LOG_CRITICAL(...) ::Log::GetEngineLogger()->critical(__VA_ARGS__)

#define GAME_LOG_TRACE(...) ::Log::GetGameLogger()->trace(__VA_ARGS__)
#define GAME_LOG_DEBUG(...) ::Log::GetGameLogger()->debug(__VA_ARGS__)
#define GAME_LOG_INFO(...) ::Log::GetGameLogger()->info(__VA_ARGS__)
#define GAME_LOG_WARN(...) ::Log::GetGameLogger()->warn(__VA_ARGS__)
#define GAME_LOG_ERROR(...) ::Log::GetGameLogger()->error(__VA_ARGS__)
#define GAME_LOG_CRITICAL(...) ::Log::GetGameLogger()->critical(__VA_ARGS__)
