// Copyright Sylar129

#pragma once

#include <string>
#include <utility>
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

enum class TileCollisionType {
  kNone,
  kSolid,
  kOneWay,
  kSlope,
};

struct TileSlope {
  float left_height = 0.0f;
  float right_height = 0.0f;
};

struct TilePhysics {
  TileCollisionType collision = TileCollisionType::kNone;
  TileSlope slope;
  bool is_climbable = false;
};

struct TileInfo {
  render::Sprite sprite;
  TilePhysics physics;
  std::string trigger_tag;
  TileInfo(render::Sprite s = render::Sprite(), TilePhysics p = TilePhysics(),
           std::string trigger = {})
      : sprite(std::move(s)),
        physics(std::move(p)),
        trigger_tag(std::move(trigger)) {}
  bool IsEmpty() const { return sprite.GetTextureId().empty(); }
};

class TileLayerComponent final : public Component {
  friend class object::GameObject;

 public:
  TileLayerComponent() = default;

  TileLayerComponent(const glm::ivec2& tile_size, const glm::ivec2& map_size,
                     std::vector<TileInfo>&& tiles);

  const TileInfo* GetTileInfoAt(const glm::ivec2& pos) const;
  const TileInfo* GetTileInfoAtWorldPos(const glm::vec2& world_pos) const;
  const TilePhysics* GetTilePhysicsAt(const glm::ivec2& pos) const;

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
  glm::vec2 offset_ = {0.0f, 0.0f};
  bool is_hidden_ = false;
};

}  // namespace engine::component
