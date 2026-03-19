// Copyright Sylar129

#pragma once

#include <vector>

#include "engine/ecs/component/tile_info.h"
#include "glm/vec2.hpp"

namespace engine::ecs {

struct TileLayerComponent {
  glm::ivec2 tile_size = {0, 0};
  glm::ivec2 map_size = {0, 0};
  std::vector<TileInfo> tiles;
  glm::vec2 offset = {0.0f, 0.0f};
  bool hidden = false;
};

}  // namespace engine::ecs