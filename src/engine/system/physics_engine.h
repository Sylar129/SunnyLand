// Copyright Sylar129

#pragma once


#include "glm/vec2.hpp"
#include "utils/math.h"
#include "utils/non_copyable.h"



namespace engine::physics {

class PhysicsEngine {
 public:
  PhysicsEngine() = default;

  DISABLE_COPY_AND_MOVE(PhysicsEngine);

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

 private:
  glm::vec2 gravity_ = {0.0f, 980.0f};
  float max_speed_ = 500.0f;
  std::optional<utils::Rect> world_bounds_;
};

}  // namespace engine::physics
