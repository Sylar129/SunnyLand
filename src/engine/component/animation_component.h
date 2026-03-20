// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace engine::render {
class Animation;
}

namespace engine::component {

struct AnimationComponent {
  std::unordered_map<std::string, std::unique_ptr<render::Animation>>
      animations_;
  render::Animation* current_animation_ = nullptr;

  float animation_timer_ = 0.0f;
  bool is_playing_ = false;
  bool is_one_shot_removal_ = false;
};

}  // namespace engine::component
