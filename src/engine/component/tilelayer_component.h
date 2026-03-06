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

namespace engine::component {

enum class TileType {
  EMPTY,
  NORMAL,
  SOLID,
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

  const TileInfo* getTileInfoAt(const glm::ivec2& pos) const;

  TileType getTileTypeAt(const glm::ivec2& pos) const;

  TileType getTileTypeAtWorldPos(const glm::vec2& world_pos) const;

  glm::ivec2 getTileSize() const { return tile_size_; }
  glm::ivec2 getMapSize() const { return map_size_; }
  glm::vec2 getWorldSize() const {
    return glm::vec2(map_size_.x * tile_size_.x, map_size_.y * tile_size_.y);
  }
  const std::vector<TileInfo>& getTiles() const { return tiles_; }
  const glm::vec2& getOffset() const { return offset_; }
  bool isHidden() const { return is_hidden_; }

  void setOffset(const glm::vec2& offset) { offset_ = offset; }
  void setHidden(bool hidden) { is_hidden_ = hidden; }

 protected:
  void Init() override;
  void Update(float, engine::core::Context&) override {}
  void Render(engine::core::Context& context) override;

 private:
  glm::ivec2 tile_size_;
  glm::ivec2 map_size_;
  std::vector<TileInfo> tiles_;
  glm::vec2 offset_ = {0.0f, 0.0f};
  bool is_hidden_ = false;
};

}  // namespace engine::component
