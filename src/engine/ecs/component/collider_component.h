// Copyright Sylar129

#pragma once

#include "glm/vec2.hpp"
#include "utils/alignment.h"

namespace engine::ecs {

struct ColliderComponent {
  glm::vec2 size = {0.0f, 0.0f};
  glm::vec2 offset = {0.0f, 0.0f};
  utils::Alignment alignment = utils::Alignment::kNone;
  bool is_trigger = false;
  bool is_active = true;
};

}  // namespace engine::ecs