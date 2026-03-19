// Copyright Sylar129

#pragma once

#include <string>

#include "glm/vec2.hpp"

namespace game::ecs {

struct EffectRequest {
  glm::vec2 center = {0.0f, 0.0f};
  std::string kind;
};

}  // namespace game::ecs