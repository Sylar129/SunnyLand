// Copyright Sylar129

#include "engine/component/tilelayer_component.h"

#include <cmath>

#include "engine/core/context.h"
#include "engine/physics/physics_engine.h"
#include "engine/render/renderer.h"
#include "utils/assert.h"
#include "utils/log.h"

namespace engine::component {

TileLayerComponent::TileLayerComponent(const glm::ivec2& tile_size,
                                       const glm::ivec2& map_size,
                                       std::vector<TileInfo>&& tiles)
    : tile_size_(tile_size), map_size_(map_size), tiles_(std::move(tiles)) {
  if (tiles_.size() != static_cast<size_t>(map_size_.x * map_size_.y)) {
    ENGINE_LOG_ERROR(
        "TileLayerComponent: Invalid tiles data size. Clearing tiles.");
    tiles_.clear();
    map_size_ = {0, 0};
  }
  ENGINE_LOG_TRACE("TileLayerComponent constructed.");
}

void TileLayerComponent::Init() {
  ENGINE_LOG_ASSERT(owner_, "TileLayerComponent Init failed: owner_ is null.");

  ENGINE_LOG_TRACE("TileLayerComponent initialized.");
}

void TileLayerComponent::Render(core::Context& context) {
  if (tile_size_.x <= 0 || tile_size_.y <= 0) {
    return;
  }

  for (int y = 0; y < map_size_.y; ++y) {
    for (int x = 0; x < map_size_.x; ++x) {
      size_t index = static_cast<size_t>(y) *
                         static_cast<size_t>(map_size_.x) +
                     static_cast<size_t>(x);

      if (index < tiles_.size() && !tiles_[index].IsEmpty()) {
        const auto& tile_info = tiles_[index];

        glm::vec2 tile_left_top_pos = {
          offset_.x + static_cast<float>(x) *
                  static_cast<float>(tile_size_.x),
          offset_.y + static_cast<float>(y) *
                  static_cast<float>(tile_size_.y)};

        if (static_cast<int>(tile_info.sprite.GetSourceRect()->h) !=
            tile_size_.y) {
          tile_left_top_pos.y -= (tile_info.sprite.GetSourceRect()->h -
                                  static_cast<float>(tile_size_.y));
        }
        context.GetRenderer().DrawSprite(context.GetCamera(), tile_info.sprite,
                                         tile_left_top_pos);
      }
    }
  }
}

void TileLayerComponent::Clean() {
  if (physics_engine_) {
    physics_engine_->UnregisterCollisionLayer(this);
  }
}

const TileInfo* TileLayerComponent::GetTileInfoAt(const glm::ivec2& pos) const {
  if (pos.x < 0 || pos.x >= map_size_.x || pos.y < 0 || pos.y >= map_size_.y) {
    ENGINE_LOG_WARN("TileLayerComponent: invalid pos: ({}, {})", pos.x, pos.y);
    return nullptr;
  }
  size_t index = static_cast<size_t>(pos.y * map_size_.x + pos.x);
  if (index < tiles_.size()) {
    return &tiles_[index];
  }
  ENGINE_LOG_WARN("TileLayerComponent: invalid index: {}", index);
  return nullptr;
}

const TileInfo* TileLayerComponent::GetTileInfoAtWorldPos(
    const glm::vec2& world_pos) const {
  glm::vec2 relative_pos = world_pos - offset_;

    int tile_x =
      static_cast<int>(std::floor(relative_pos.x / static_cast<float>(tile_size_.x)));
    int tile_y =
      static_cast<int>(std::floor(relative_pos.y / static_cast<float>(tile_size_.y)));

  return GetTileInfoAt(glm::ivec2{tile_x, tile_y});
}

const TilePhysics* TileLayerComponent::GetTilePhysicsAt(
    const glm::ivec2& pos) const {
  const TileInfo* info = GetTileInfoAt(pos);
  return info ? &info->physics : nullptr;
}

}  // namespace engine::component
