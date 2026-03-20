// Copyright Sylar129

#pragma once

#include "glm/vec2.hpp"

namespace engine::component {

struct TransformComponent final {
  glm::vec2 position = {0.0f, 0.0f};
  glm::vec2 scale = {1.0f, 1.0f};
  // degree
  float rotation = 0.0f;
};

}  // namespace engine::component
