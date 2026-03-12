// Copyright Sylar129

#include "game/component/ai_component.h"

#include "engine/component/animation_component.h"
#include "engine/component/health_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/transform_component.h"
#include "engine/object/game_object.h"
#include "engine/utils/assert.h"
#include "game/component/ai/ai_behavior.h"
#include "log.h"

namespace game::component {

void AIComponent::Init() {
  GAME_ASSERT(owner_, "PlayerComponent must have an owner GameObject");

  transform_component_ =
      owner_->GetComponent<engine::component::TransformComponent>();
  physics_component_ =
      owner_->GetComponent<engine::component::PhysicsComponent>();
  sprite_component_ =
      owner_->GetComponent<engine::component::SpriteComponent>();
  animation_component_ =
      owner_->GetComponent<engine::component::AnimationComponent>();

  if (!transform_component_ || !physics_component_ || !sprite_component_ ||
      !animation_component_) {
    GAME_ERROR("GameObject '{}' 上的 AIComponent 缺少必需的组件",
               owner_->GetName());
  }
}

void AIComponent::Update(float delta_time, engine::core::Context&) {
  if (current_behavior_) {
    current_behavior_->Update(delta_time, *this);
  } else {
    GAME_WARN("GameObject '{}' 上的 AIComponent 没有设置行为。",
              owner_ ? owner_->GetName() : "Unknown");
  }
}

void AIComponent::SetBehavior(std::unique_ptr<ai::AIBehavior> behavior) {
  current_behavior_ = std::move(behavior);
  GAME_DEBUG("GameObject '{}' 上的 AIComponent 设置了新的行为。",
             owner_ ? owner_->GetName() : "Unknown");
  if (current_behavior_) {
    current_behavior_->Enter(*this);
  }
}

bool AIComponent::TakeDamage(int damage) {
  bool success = false;
  if (auto* health_component =
          GetOwner()->GetComponent<engine::component::HealthComponent>();
      health_component) {
    success = health_component->TakeDamage(damage);
  }
  return success;
}

bool AIComponent::IsAlive() const {
  if (auto* health_component =
          GetOwner()->GetComponent<engine::component::HealthComponent>();
      health_component) {
    return health_component->IsAlive();
  }
  return true;
}

}  // namespace game::component
