// Copyright Sylar129

#pragma once

#include <cstdint>

#include "glm/vec2.hpp"

namespace engine::component {

using CollisionFlag = uint8_t;
static constexpr CollisionFlag kCollidedBelow = 1 << 0;
static constexpr CollisionFlag kCollidedAbove = 1 << 1;
static constexpr CollisionFlag kCollidedLeft = 1 << 2;
static constexpr CollisionFlag kCollidedRight = 1 << 3;
static constexpr CollisionFlag kCollidedLadder = 1 << 4;
static constexpr CollisionFlag kIsOnTopLadder = 1 << 5;

struct PhysicsComponent final {
  glm::vec2 velocity = {0.0f, 0.0f};
  glm::vec2 force = {0.0f, 0.0f};
  float mass = 1.0f;
  bool use_gravity = true;
  bool enabled = true;

  CollisionFlag collision_flag_;
};

}  // namespace engine::component
