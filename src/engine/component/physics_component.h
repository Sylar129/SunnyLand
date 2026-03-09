// Copyright Sylar129

#pragma once

#include "component.h"
#include "engine/utils/non_copyable.h"
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

  DISABLE_COPY_AND_MOVE(PhysicsComponent);

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
};

}  // namespace engine::component
