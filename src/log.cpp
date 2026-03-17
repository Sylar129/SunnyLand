// Copyright Sylar129

#include "log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

std::shared_ptr<spdlog::logger> Log::engine_logger_;
std::shared_ptr<spdlog::logger> Log::game_logger_;

void Log::Init() {
  spdlog::set_pattern("%^[%T] %n: %v%$");
  engine_logger_ = spdlog::stdout_color_mt("Engine");
  engine_logger_->set_level(spdlog::level::trace);

  game_logger_ = spdlog::stdout_color_mt("Game");
  game_logger_->set_level(spdlog::level::trace);
}
