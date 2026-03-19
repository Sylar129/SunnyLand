// Copyright Sylar129

#pragma once

#include "engine/render/sprite.h"
#include "glm/vec2.hpp"

namespace engine::ecs {

struct ParallaxComponent {
  render::Sprite sprite;
  glm::vec2 scroll_factor = {1.0f, 1.0f};
  glm::bvec2 repeat = {false, false};
  bool hidden = false;
};

}  // namespace engine::ecs