// Copyright Sylar129

#pragma once

#include <vector>

#include "engine/object/game_object.h"
#include "engine/utils/non_copyable.h"
#include "glm/vec2.hpp"

namespace engine::component {
class PhysicsComponent;
}

namespace engine::physics {

class PhysicsEngine {
 public:
  PhysicsEngine() = default;

  DISABLE_COPY_AND_MOVE(PhysicsEngine);

  void RegisterComponent(engine::component::PhysicsComponent* component);
  void UnregisterComponent(engine::component::PhysicsComponent* component);

  void Update(float delta_time);

  void SetGravity(const glm::vec2& gravity) { gravity_ = gravity; }
  const glm::vec2& GetGravity() const { return gravity_; }
  void SetMaxSpeed(float max_speed) { max_speed_ = max_speed; }
  float GetMaxSpeed() const { return max_speed_; }

  const auto& GetCollisionPairs() const { return collision_pairs_; };

 private:
  void CheckObjectCollisions();

  std::vector<engine::component::PhysicsComponent*> components_;
  glm::vec2 gravity_ = {0.0f, 980.0f};
  float max_speed_ = 500.0f;

  std::vector<
      std::pair<engine::object::GameObject*, engine::object::GameObject*>>
      collision_pairs_;
};

}  // namespace engine::physics
