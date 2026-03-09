// Copyright Sylar129

#include "engine/physics/physics_engine.h"

#include "engine/component/physics_component.h"
#include "engine/component/transform_component.h"
#include "engine/utils/assert.h"
#include "glm/common.hpp"
#include "log.h"

namespace engine::physics {

void PhysicsEngine::RegisterComponent(
    engine::component::PhysicsComponent* component) {
  ENGINE_ASSERT(component, "PhysicsComponent is null.");

  components_.push_back(component);
  ENGINE_TRACE("Register PhysicsComponent complete.");
}

void PhysicsEngine::UnregisterComponent(
    engine::component::PhysicsComponent* component) {
  std::erase(components_, component);
  ENGINE_TRACE("Unregister PhysicsComponent complete.");
}

void PhysicsEngine::Update(float delta_time) {
  for (auto* pc : components_) {
    if (!pc->isEnabled()) {
      continue;
    }

    if (pc->isUseGravity()) {
      pc->addForce(gravity_ * pc->getMass());
    }

    pc->velocity_ += (pc->getForce() / pc->getMass()) * delta_time;
    pc->clearForce();

    auto* tc = pc->getTransform();
    if (tc) {
      tc->Translate(pc->velocity_ * delta_time);
    }

    pc->velocity_ = glm::clamp(pc->velocity_, -max_speed_, max_speed_);
  }
}

}  // namespace engine::physics
