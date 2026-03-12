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

enum class TileType {
  EMPTY,
  NORMAL,
  SOLID,
  UNISOLID,
  SLOPE_0_1,  ///< @brief Slope tile, height: left 0,   right 1
  SLOPE_1_0,  ///< @brief Slope tile, height: left 1,   right 0
  SLOPE_0_2,  ///< @brief Slope tile, height: left 0,   right 0.5
  SLOPE_2_1,  ///< @brief Slope tile, height: left 0.5, right 1
  SLOPE_1_2,  ///< @brief Slope tile, height: left 1,   right 0.5
  SLOPE_2_0,  ///< @brief Slope tile, height: left 0.5, right 0
  HAZARD,     ///< @brief damaging tile, e.g. spikes, lava, etc.
  LADDER,     ///< @brief ladder tile that allows climbing
};

struct TileInfo {
  render::Sprite sprite;
  TileType type;
  TileInfo(render::Sprite s = render::Sprite(), TileType t = TileType::EMPTY)
      : sprite(std::move(s)), type(t) {}
};

class TileLayerComponent final : public Component {
  friend class engine::object::GameObject;

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
  const glm::vec2& GetOffset() const { return offset_; }
  bool IsHidden() const { return is_hidden_; }

  void SetOffset(const glm::vec2& offset) { offset_ = offset; }
  void SetHidden(bool hidden) { is_hidden_ = hidden; }

  void setPhysicsEngine(engine::physics::PhysicsEngine* physics_engine) {
    physics_engine_ = physics_engine;
  }

 protected:
  void Init() override;
  void Update(float, engine::core::Context&) override {}
  void Render(engine::core::Context& context) override;
  void Clean() override;

 private:
  engine::physics::PhysicsEngine* physics_engine_ = nullptr;
  glm::ivec2 tile_size_;
  glm::ivec2 map_size_;
  std::vector<TileInfo> tiles_;
  glm::vec2 offset_ = {0.0f, 0.0f};
  bool is_hidden_ = false;
};

}  // namespace engine::component
