// Copyright Sylar129

#pragma once

#include <optional>
#include <vector>

#include "engine/ecs/components.h"
#include "entt/entt.hpp"
#include "utils/math.h"

namespace engine::core {
class Context;
}

namespace engine::ecs {

struct CollisionPair {
  entt::entity first = entt::null;
  entt::entity second = entt::null;
};

struct TileTriggerEvent {
  entt::entity entity = entt::null;
  TileType tile_type = TileType::kEmpty;
};

glm::vec2 GetAlignmentOffset(utils::Alignment alignment,
                             const glm::vec2& size,
                             const glm::vec2& scale);
utils::Rect GetWorldAabb(const TransformComponent& transform,
                         const ColliderComponent& collider);
bool HasCollisionFlag(const PhysicsComponent& physics,
                      PhysicsComponent::CollisionFlag flag);
void SetCollisionFlag(PhysicsComponent& physics,
                      PhysicsComponent::CollisionFlag flag);
void ClearCollisionFlags(PhysicsComponent& physics);
void DestroyPendingEntities(entt::registry& registry);

class AnimationSystem final {
 public:
  void Update(entt::registry& registry, float delta_time) const;
};

class RenderSystem final {
 public:
  void Render(entt::registry& registry, core::Context& context) const;
};

class PhysicsSystem final {
 public:
  void Update(entt::registry& registry, float delta_time);

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

  const std::vector<CollisionPair>& GetCollisionPairs() const {
    return collision_pairs_;
  }
  const std::vector<TileTriggerEvent>& GetTileTriggerEvents() const {
    return tile_trigger_events_;
  }

 private:
  void CheckObjectCollisions(entt::registry& registry);
  void CheckTileTriggers(entt::registry& registry);
  void ResolveTileCollisions(entt::registry& registry, entt::entity entity,
                             PhysicsComponent& physics,
                             TransformComponent& transform,
                             float delta_time);
  void ResolveSolidObjectCollisions(entt::registry& registry,
                                    entt::entity moving_entity,
                                    entt::entity solid_entity);
  float GetTileHeightAtWidth(float width, TileType type,
                             const glm::vec2& tile_size) const;
  void ApplyWorldBounds(entt::registry& registry, entt::entity entity,
                        PhysicsComponent& physics,
                        TransformComponent& transform) const;

  glm::vec2 gravity_ = {0.0f, 980.0f};
  float max_speed_ = 500.0f;
  std::optional<utils::Rect> world_bounds_;
  std::vector<CollisionPair> collision_pairs_;
  std::vector<TileTriggerEvent> tile_trigger_events_;
};

}  // namespace engine::ecs