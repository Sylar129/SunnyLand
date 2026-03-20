// Copyright Sylar129

#pragma once

#include <vector>

#include "engine/component/tilelayer_component.h"
#include "engine/object/game_object.h"
#include "glm/vec2.hpp"
#include "utils/math.h"
#include "utils/non_copyable.h"

namespace engine::component {
class PhysicsComponent;
class TileLayerComponent;
}  // namespace engine::component

namespace engine::physics {

class PhysicsEngine {
 public:
  PhysicsEngine() = default;

  DISABLE_COPY_AND_MOVE(PhysicsEngine);

  void RegisterComponent(component::PhysicsComponent* component);
  void UnregisterComponent(component::PhysicsComponent* component);

  void RegisterCollisionLayer(component::TileLayerComponent* layer);
  void UnregisterCollisionLayer(component::TileLayerComponent* layer);

  void Update(float delta_time);

  void SetGravity(const glm::vec2& gravity) { gravity_ = gravity; }
  const glm::vec2& GetGravity() const { return gravity_; }
  void SetMaxSpeed(float max_speed) { max_speed_ = max_speed; }
  float GetMaxSpeed() const { return max_speed_; }
  void SetWorldBounds(const utils::Rect& world_bounds) {
    world_bounds_ = world_bounds;
  }
  const std::optional<utils::Rect>& GetWorldBounds() const {
    return world_bounds_;
  }

  const auto& GetCollisionPairs() const { return collision_pairs_; };
  const auto& getTileTriggerEvents() const { return tile_trigger_events_; };

 private:
  void CheckObjectCollisions();
  void CheckTileTriggers();
  void ResolveTileCollisions(component::PhysicsComponent* pc, float delta_time);
  void ResolveSolidObjectCollisions(object::GameObject* move_obj,
                                    object::GameObject* solid_obj);

  float GetTileHeightAtWidth(float width, component::TileType type,
                             glm::vec2 tile_size);

  void ApplyWorldBounds(component::PhysicsComponent* pc);

  std::vector<component::PhysicsComponent*> components_;
  glm::vec2 gravity_ = {0.0f, 980.0f};
  float max_speed_ = 500.0f;
  std::optional<utils::Rect> world_bounds_;

  std::vector<std::pair<object::GameObject*, object::GameObject*>>
      collision_pairs_;
  std::vector<component::TileLayerComponent*> collision_tile_layers_;

  std::vector<std::pair<object::GameObject*, component::TileType>>
      tile_trigger_events_;
};

}  // namespace engine::physics
