// Copyright Sylar129

#include "engine/component/physics_component.h"

#include "engine/component/transform_component.h"
#include "engine/object/game_object.h"
#include "engine/physics/physics_engine.h"
#include "utils/assert.h"
#include "utils/log.h"

namespace engine::component {

PhysicsComponent::PhysicsComponent(
    engine::physics::PhysicsEngine* physics_engine, bool use_gravity,
    float mass)
    : physics_engine_(physics_engine),
      mass_(mass > 0.0f ? mass : 1.0f),
      use_gravity_(use_gravity) {
  ENGINE_LOG_ASSERT(
      physics_engine_,
      "In PhysicsComponent constructor, PhysicsEngine pointer cannot be "
      "nullptr!");

  ENGINE_LOG_TRACE(
      "Physics component created successfully, mass: {}, use gravity: {}",
      mass_, use_gravity_);
}

void PhysicsComponent::Init() {
  ENGINE_LOG_ASSERT(
      owner_,
      "A GameObject is required as owner before physics component "
      "initialization!");

  transform_ = owner_->GetComponent<TransformComponent>();
  if (!transform_) {
    ENGINE_LOG_WARN(
        "TransformComponent not found on the same GameObject during physics "
        "component initialization.");
  }
  physics_engine_->RegisterComponent(this);
  ENGINE_LOG_TRACE("Physics component initialization completed.");
}

void PhysicsComponent::Clean() {
  physics_engine_->UnregisterComponent(this);
  ENGINE_LOG_TRACE("Physics component cleanup completed.");
}

}  // namespace engine::component
