// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "engine/render/animation.h"

namespace engine::ecs {

struct AnimationComponent {
  std::unordered_map<std::string, std::shared_ptr<render::Animation>>
      animations;
  std::string current_animation;
  float animation_timer = 0.0f;
  bool is_playing = false;
  bool is_one_shot_removal = false;
};

}  // namespace engine::ecs