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

  /**
   * @brief 构造函数
   * @param tile_size 单个瓦片尺寸（像素）
   * @param map_size 地图尺寸（瓦片数）
   * @param tiles 初始化瓦片数据的容器 (会被移动)
   */
  TileLayerComponent(const glm::ivec2& tile_size, const glm::ivec2& map_size,
                     std::vector<TileInfo>&& tiles);

  /**
   * @brief 根据瓦片坐标获取瓦片信息
   * @param pos 瓦片坐标 (0 <= x < map_size_.x, 0 <= y < map_size_.y)
   * @return const TileInfo* 指向瓦片信息的指针，如果坐标无效则返回 nullptr
   */
  const TileInfo* getTileInfoAt(const glm::ivec2& pos) const;

  /**
   * @brief 根据瓦片坐标获取瓦片类型
   * @param pos 瓦片坐标 (0 <= x < map_size_.x, 0 <= y < map_size_.y)
   * @return TileType 瓦片类型，如果坐标无效则返回 TileType::EMPTY
   */
  TileType getTileTypeAt(const glm::ivec2& pos) const;

  /**
   * @brief 根据世界坐标获取瓦片类型
   * @param world_pos 世界坐标
   * @return TileType 瓦片类型，如果坐标无效或对应空瓦片则返回 TileType::EMPTY
   */
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
  glm::ivec2 tile_size_;         ///< @brief 单个瓦片尺寸（像素）
  glm::ivec2 map_size_;          ///< @brief 地图尺寸（瓦片数）
  std::vector<TileInfo> tiles_;  ///< @brief 存储所有瓦片信息 (按"行主序"存储,
                                 ///< index = y * map_width_ + x)
  glm::vec2 offset_ = {
      0.0f, 0.0f};  ///< @brief 瓦片层在世界中的偏移量
                    ///< (瓦片层通常不需要缩放及旋转，因此不引入Transform组件)
                    // offset_ 最好也保持默认的0，以免增加不必要的复杂性
  bool is_hidden_ = false;  ///< @brief 是否隐藏（不渲染）
};

}  // namespace engine::component
