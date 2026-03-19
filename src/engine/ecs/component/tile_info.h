// Copyright Sylar129

#pragma once

#include "engine/ecs/component/tile_type.h"
#include "engine/render/sprite.h"

namespace engine::ecs {

struct TileInfo {
  render::Sprite sprite;
  TileType type = TileType::kEmpty;
};

}  // namespace engine::ecs