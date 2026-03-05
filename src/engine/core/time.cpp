// Copyright Sylar129

#include "engine/core/time.h"

#include "SDL3/SDL_timer.h"
#include "log.h"

namespace engine::core {

Time::Time() {
  last_time_ = SDL_GetTicksNS();
  frame_start_time_ = last_time_;
  ENGINE_TRACE("Time init. Last time: {}", last_time_);
}

void Time::Update() {
  frame_start_time_ = SDL_GetTicksNS();
  auto current_delta_time =
      static_cast<float>(frame_start_time_ - last_time_) / 1000000000.0;
  if (target_frame_time_ > 0.0) {
    LimitFrameRate(current_delta_time);
  } else {
    delta_time_ = current_delta_time;
  }

  last_time_ = SDL_GetTicksNS();
}

void Time::LimitFrameRate(float current_delta_time) {
  if (current_delta_time < target_frame_time_) {
    float time_to_wait = target_frame_time_ - current_delta_time;
    Uint64 ns_to_wait = static_cast<Uint64>(time_to_wait * 1000000000.0);
    SDL_DelayNS(ns_to_wait);
    delta_time_ =
        static_cast<float>(SDL_GetTicksNS() - last_time_) / 1000000000.0;
  }
}

float Time::GetDeltaTime() const { return delta_time_ * time_scale_; }

float Time::GetUnscaledDeltaTime() const { return delta_time_; }

void Time::SetTimeScale(float scale) {
  if (scale < 0.0) {
    ENGINE_WARN("Time scale can't be negative. Clamping to 0.");
    scale = 0.0;
  }
  time_scale_ = scale;
}

float Time::GetTimeScale() const { return time_scale_; }

void Time::SetTargetFps(int fps) {
  if (fps < 0) {
    ENGINE_WARN("Target FPS can't be negative. Setting to 0 (unlimited).");
    target_fps_ = 0;
  } else {
    target_fps_ = fps;
  }

  if (target_fps_ > 0) {
    target_frame_time_ = 1.0 / static_cast<float>(target_fps_);
    ENGINE_INFO("Setting Target FPS: {} (Frame time: {:.6f}s)", target_fps_,
                target_frame_time_);
  } else {
    target_frame_time_ = 0.0;
    ENGINE_INFO("Setting Target FPS: Unlimited");
  }
}

int Time::GetTargetFps() const { return target_fps_; }

}  // namespace engine::core
