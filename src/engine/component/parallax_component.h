// Copyright Sylar129

#pragma once

#include "engine/render/sprite.h"
#include "glm/vec2.hpp"

namespace engine::component {

struct ParallaxComponent final {
  render::Sprite sprite;
  glm::vec2 scroll_factor;
  glm::bvec2 repeat;
  bool is_hidden = false;
};

}  // namespace engine::component
