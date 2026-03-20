// Copyright Sylar129

#pragma once

#include "engine/system/system.h"
#include "glm/vec2.hpp"

namespace engine::physics {

class PhysicsEngine : public system::System {
 public:
  PhysicsEngine() = default;

  void Update(float delta_time);

 private:
  glm::vec2 gravity_ = {0.0f, 980.0f};
  float max_speed_ = 500.0f;
};

}  // namespace engine::physics
