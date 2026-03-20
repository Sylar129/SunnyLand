// Copyright Sylar129

#pragma once

#include <vector>

#include "engine/component/component.h"
#include "engine/render/sprite.h"
#include "glm/vec2.hpp"

namespace engine::render {
class Sprite;
}

namespace engine::core {
class Context;
}

namespace engine::physics {
class PhysicsEngine;
}

namespace engine::component {

using TileType = std::string;

struct TileInfo {
  render::Sprite sprite;
  TileType type;
};

class TileLayerComponent final : public Component {
  friend class object::GameObject;

 public:
  TileLayerComponent() = default;

  TileLayerComponent(const glm::ivec2& tile_size, const glm::ivec2& map_size,
                     std::vector<TileInfo>&& tiles);

  const TileInfo* GetTileInfoAt(const glm::ivec2& pos) const;

  TileType GetTileTypeAt(const glm::ivec2& pos) const;

  TileType GetTileTypeAtWorldPos(const glm::vec2& world_pos) const;

  glm::ivec2 GetTileSize() const { return tile_size_; }
  glm::ivec2 GetMapSize() const { return map_size_; }
  glm::vec2 GetWorldSize() const {
    return glm::vec2(map_size_.x * tile_size_.x, map_size_.y * tile_size_.y);
  }
  const std::vector<TileInfo>& GetTiles() const { return tiles_; }

  void SetPhysicsEngine(physics::PhysicsEngine* physics_engine) {
    physics_engine_ = physics_engine;
  }

 protected:
  void Init() override;
  void Update(float, core::Context&) override {}
  void Render(core::Context& context) override;
  void Clean() override;

 private:
  physics::PhysicsEngine* physics_engine_ = nullptr;
  glm::ivec2 tile_size_;
  glm::ivec2 map_size_;
  std::vector<TileInfo> tiles_;
};

}  // namespace engine::component
