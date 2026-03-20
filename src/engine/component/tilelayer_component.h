// Copyright Sylar129

#pragma once

#include <vector>

#include "engine/render/sprite.h"
#include "glm/vec2.hpp"

namespace engine::render {
class Sprite;
}

namespace engine::component {

using TileType = std::string;

struct TileInfo {
  render::Sprite sprite;
  TileType type;
};

struct TileLayerComponent final {
  glm::ivec2 tile_size_;
  glm::ivec2 map_size_;
  std::vector<TileInfo> tiles_;
};

}  // namespace engine::component
