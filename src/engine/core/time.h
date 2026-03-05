// Copyright Sylar129

#pragma once

#include "SDL3/SDL_stdinc.h"

namespace engine::core {

class Time final {
 public:
  Time();

  Time(const Time&) = delete;
  Time& operator=(const Time&) = delete;
  Time(Time&&) = delete;
  Time& operator=(Time&&) = delete;

  void Update();

  float GetDeltaTime() const;
  float GetUnscaledDeltaTime() const;

  void SetTimeScale(float scale);
  float GetTimeScale() const;

  void SetTargetFps(int fps);
  int GetTargetFps() const;

 private:
  void LimitFrameRate(float current_delta_time);

  Uint64 last_time_ = 0;
  Uint64 frame_start_time_ = 0;
  float delta_time_ = 0.0;
  float time_scale_ = 1.0;

  int target_fps_ = 0;  // 0 means no limit
  float target_frame_time_ = 0.0;
};

}  // namespace engine::core
