// Copyright Sylar129

#include "engine/physics/physics_engine.h"

#include "engine/component/collider_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/transform_component.h"
#include "engine/physics/collision.h"
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
  collision_pairs_.clear();

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

  checkObjectCollisions();
}

void PhysicsEngine::checkObjectCollisions() {
  // 两层循环遍历所有包含物理组件的 GameObject
  for (size_t i = 0; i < components_.size(); ++i) {
    auto* pc_a = components_[i];
    if (!pc_a || !pc_a->IsEnabled()) continue;
    auto* obj_a = pc_a->GetOwner();
    if (!obj_a) continue;
    auto* cc_a = obj_a->GetComponent<engine::component::ColliderComponent>();
    if (!cc_a || !cc_a->isActive()) continue;

    for (size_t j = i + 1; j < components_.size(); ++j) {
      auto* pc_b = components_[j];
      if (!pc_b || !pc_b->IsEnabled()) continue;
      auto* obj_b = pc_b->GetOwner();
      if (!obj_b) continue;
      auto* cc_b = obj_b->GetComponent<engine::component::ColliderComponent>();
      if (!cc_b || !cc_b->isActive()) continue;
      /* --- 通过保护性测试后，正式执行逻辑 --- */

      if (collision::checkCollision(*cc_a, *cc_b)) {
        // TODO: 并不是所有碰撞都需要插入collision_pairs_，未来会添加过滤条件
        // 记录碰撞对
        collision_pairs_.emplace_back(obj_a, obj_b);
      }
    }
  }
}

}  // namespace engine::physics
