// Copyright Sylar129

#pragma once

#include "glm/vec2.hpp"

namespace engine::component {

struct ColliderComponent final {
  glm::vec2 size = {0.0f, 0.0f};
  glm::vec2 offset = {0.0f, 0.0f};

  bool is_trigger = false;
  bool is_active = true;
};

}  // namespace engine::component
