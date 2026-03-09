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
    if (!pc->IsEnabled()) {
      continue;
    }

    if (pc->IsUseGravity()) {
      pc->AddForce(gravity_ * pc->GetMass());
    }

    pc->velocity_ += (pc->GetForce() / pc->GetMass()) * delta_time;
    pc->velocity_ = glm::clamp(pc->velocity_, -max_speed_, max_speed_);
    pc->ClearForce();

    auto* tc = pc->GetTransform();
    if (tc) {
      tc->Translate(pc->velocity_ * delta_time);
    }
  }
}

}  // namespace engine::physics
