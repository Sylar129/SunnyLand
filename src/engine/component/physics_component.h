// Copyright Sylar129

#pragma once

#include <cstdint>

#include "engine/component/component.h"
#include "glm/vec2.hpp"

namespace engine::physics {
class PhysicsEngine;
}

namespace engine::component {
class TransformComponent;

class PhysicsComponent final : public Component {
  friend class engine::object::GameObject;

 public:
  PhysicsComponent(engine::physics::PhysicsEngine* physics_engine,
                   bool use_gravity = true, float mass = 1.0f);
  ~PhysicsComponent() override = default;

  void AddForce(const glm::vec2& force) {
    if (enabled_) force_ += force;
  }
  void ClearForce() { force_ = {0.0f, 0.0f}; }
  const glm::vec2& GetForce() const { return force_; }
  float GetMass() const { return mass_; }
  bool IsEnabled() const { return enabled_; }
  bool IsUseGravity() const { return use_gravity_; }

  void SetEnabled(bool enabled) { enabled_ = enabled; }
  void SetMass(float mass) { mass_ = (mass > 0.0f) ? mass : 1.0f; }
  void SetUseGravity(bool use_gravity) { use_gravity_ = use_gravity; }
  void SetVelocity(const glm::vec2& velocity) { velocity_ = velocity; }
  const glm::vec2& GetVelocity() const { return velocity_; }
  TransformComponent* GetTransform() const { return transform_; }

  using CollisionFlag = uint8_t;
  static constexpr CollisionFlag kCollidedBelow = 1 << 0;
  static constexpr CollisionFlag kCollidedAbove = 1 << 1;
  static constexpr CollisionFlag kCollidedLeft = 1 << 2;
  static constexpr CollisionFlag kCollidedRight = 1 << 3;
  void ResetCollisionFlag() { collision_flag_ = 0; }

  void SetCollidedBelow() { collision_flag_ |= kCollidedBelow; }
  void SetCollidedAbove() { collision_flag_ |= kCollidedAbove; }
  void SetCollidedLeft() { collision_flag_ |= kCollidedLeft; }
  void SetCollidedRight() { collision_flag_ |= kCollidedRight; }

  bool HasCollidedBelow() const { return collision_flag_ & kCollidedBelow; }
  bool HasCollidedAbove() const { return collision_flag_ & kCollidedAbove; }
  bool HasCollidedLeft() const { return collision_flag_ & kCollidedLeft; }
  bool HasCollidedRight() const { return collision_flag_ & kCollidedRight; }

  glm::vec2 velocity_ = {0.0f, 0.0f};

 private:
  void Init() override;
  void Update(float, engine::core::Context&) override {}
  void Clean() override;

  engine::physics::PhysicsEngine* physics_engine_ = nullptr;
  TransformComponent* transform_ = nullptr;

  glm::vec2 force_ = {0.0f, 0.0f};
  float mass_ = 1.0f;
  bool use_gravity_ = true;
  bool enabled_ = true;

  CollisionFlag collision_flag_;
};

}  // namespace engine::component
