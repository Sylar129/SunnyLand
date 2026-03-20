// Copyright Sylar129

#pragma once

#include "engine/render/texture.h"
#include "glm/vec2.hpp"

namespace engine::component {

struct ParallaxComponent final {
  render::Texture texture;
  glm::vec2 scroll_factor;
  glm::bvec2 repeat;
  bool is_hidden = false;
};

}  // namespace engine::component
