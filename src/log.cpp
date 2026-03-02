// Copyright Sylar129

#include "log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"


std::shared_ptr<spdlog::logger> Log::s_EngineLogger;
std::shared_ptr<spdlog::logger> Log::s_GameLogger;

void Log::Init() {
  spdlog::set_pattern("%^[%T] %n: %v%$");
  s_EngineLogger = spdlog::stdout_color_mt("Solar");
  s_EngineLogger->set_level(spdlog::level::trace);

  s_GameLogger = spdlog::stdout_color_mt("APP");
  s_GameLogger->set_level(spdlog::level::trace);
}
