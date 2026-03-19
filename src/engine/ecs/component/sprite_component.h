// Copyright Sylar129

#pragma once

#include "engine/render/sprite.h"
#include "glm/vec2.hpp"
#include "utils/alignment.h"

namespace engine::ecs {

struct SpriteComponent {
  render::Sprite sprite;
  utils::Alignment alignment = utils::Alignment::kNone;
  glm::vec2 sprite_size = {0.0f, 0.0f};
  glm::vec2 offset = {0.0f, 0.0f};
  bool hidden = false;
};

}  // namespace engine::ecs