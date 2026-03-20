// Copyright Sylar129

#pragma once

#include <vector>

#include "engine/render/texture.h"
#include "glm/vec2.hpp"

namespace engine::render {
class Texture;
}

namespace engine::component {

using TileType = std::string;

struct TileInfo {
  render::Texture sprite;
  TileType type;
};

struct TileLayerComponent final {
  glm::ivec2 tile_size_;
  glm::ivec2 map_size_;
  std::vector<TileInfo> tiles_;
};

}  // namespace engine::component
