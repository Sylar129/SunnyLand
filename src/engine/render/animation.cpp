// Copyright Sylar129

#include "engine/render/animation.h"

#include "glm/common.hpp"

namespace engine::render {

Animation::Animation(const std::string& name, bool loop)
    : name_(name), loop_(loop) {}

void Animation::AddFrame(const SDL_FRect& source_rect, float duration) {
  if (duration <= 0.0f) {
    return;
  }
  frames_.push_back({source_rect, duration});
  total_duration_ += duration;
}

AnimationFrame Animation::GetFrame(float time) const {
  if (frames_.empty()) {
    return {};
  }

  float current_time = time;

  if (loop_ && total_duration_ > 0.0f) {
    current_time = glm::mod(time, total_duration_);
  } else {
    if (current_time >= total_duration_) {
      return frames_.back();
    }
  }

  float accumulated_time = 0.0f;
  for (const auto& frame : frames_) {
    accumulated_time += frame.duration;
    if (current_time < accumulated_time) {
      return frame;
    }
  }

  return frames_.back();
}

}  // namespace engine::render
