// Copyright Sylar129

#pragma once

#include <vector>

#include "engine/object/game_object.h"
#include "engine/utils/math.h"
#include "engine/utils/non_copyable.h"
#include "glm/vec2.hpp"

namespace engine::component {
class PhysicsComponent;
class TileLayerComponent;
enum class TileType;
}

namespace engine::physics {

class PhysicsEngine {
 public:
  PhysicsEngine() = default;

  DISABLE_COPY_AND_MOVE(PhysicsEngine);

  void RegisterComponent(engine::component::PhysicsComponent* component);
  void UnregisterComponent(engine::component::PhysicsComponent* component);

  void RegisterCollisionLayer(engine::component::TileLayerComponent* layer);
  void UnregisterCollisionLayer(engine::component::TileLayerComponent* layer);

  void Update(float delta_time);

  void SetGravity(const glm::vec2& gravity) { gravity_ = gravity; }
  const glm::vec2& GetGravity() const { return gravity_; }
  void SetMaxSpeed(float max_speed) { max_speed_ = max_speed; }
  float GetMaxSpeed() const { return max_speed_; }
  void SetWorldBounds(const engine::utils::Rect& world_bounds) {
    world_bounds_ = world_bounds;
  }
  const std::optional<engine::utils::Rect>& GetWorldBounds() const {
    return world_bounds_;
  }

  const auto& GetCollisionPairs() const { return collision_pairs_; };
  /// @brief 获取本帧检测到的所有瓦片触发事件。(此列表在每次 update 开始时清空)
  const std::vector<
      std::pair<engine::object::GameObject*, engine::component::TileType>>&
  getTileTriggerEvents() const {
    return tile_trigger_events_;
  };

 private:
  void CheckObjectCollisions();
  /**
   * @brief
   * 检测所有游戏对象与瓦片层的触发器类型瓦片碰撞，并记录触发事件。(位移处理完毕后再调用)
   */
  void checkTileTriggers();
  void ResolveTileCollisions(engine::component::PhysicsComponent* pc,
                             float delta_time);
  void ResolveSolidObjectCollisions(engine::object::GameObject* move_obj,
                                    engine::object::GameObject* solid_obj);

  float GetTileHeightAtWidth(float width, engine::component::TileType type,
                             glm::vec2 tile_size);

  void ApplyWorldBounds(engine::component::PhysicsComponent* pc);

  std::vector<engine::component::PhysicsComponent*> components_;
  glm::vec2 gravity_ = {0.0f, 980.0f};
  float max_speed_ = 500.0f;
  std::optional<engine::utils::Rect> world_bounds_;

  std::vector<
      std::pair<engine::object::GameObject*, engine::object::GameObject*>>
      collision_pairs_;
  std::vector<engine::component::TileLayerComponent*> collision_tile_layers_;
  /// @brief 存储本帧发生的瓦片触发事件 (GameObject*, 触发的瓦片类型, 每次
  /// update 开始时清空)
  std::vector<
      std::pair<engine::object::GameObject*, engine::component::TileType>>
      tile_trigger_events_;
};

}  // namespace engine::physics
