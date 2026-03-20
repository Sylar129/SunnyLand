// Copyright Sylar129

#pragma once

#include "SDL3/SDL_rect.h"
#include "engine/render/sprite.h"
#include "glm/vec2.hpp"
#include "utils/alignment.h"

namespace engine::component {

struct SpriteComponent final {
  render::Sprite sprite_;
  utils::Alignment alignment_ = utils::Alignment::kNone;
  glm::vec2 sprite_size_ = {0.0f, 0.0f};
  glm::vec2 offset_ = {0.0f, 0.0f};
  bool is_hidden_ = false;
};

}  // namespace engine::component
