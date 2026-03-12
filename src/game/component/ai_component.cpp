// Copyright Sylar129

#include "game/component/ai_component.h"

#include <spdlog/spdlog.h>

#include "engine/component/animation_component.h"
#include "engine/component/health_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/transform_component.h"
#include "engine/object/game_object.h"
#include "game/component/ai/ai_behavior.h"

namespace game::component {

void AIComponent::Init() {
  if (!owner_) {
    spdlog::error("PlayerComponent 没有所属游戏对象!");
    return;
  }

  // 获取并缓存必要的组件指针
  transform_component_ =
      owner_->GetComponent<engine::component::TransformComponent>();
  physics_component_ =
      owner_->GetComponent<engine::component::PhysicsComponent>();
  sprite_component_ =
      owner_->GetComponent<engine::component::SpriteComponent>();
  animation_component_ =
      owner_->GetComponent<engine::component::AnimationComponent>();

  // 检查是否所有必需的组件都存在
  if (!transform_component_ || !physics_component_ || !sprite_component_ ||
      !animation_component_) {
    spdlog::error("GameObject '{}' 上的 AIComponent 缺少必需的组件",
                  owner_->GetName());
  }
}

void AIComponent::Update(float delta_time, engine::core::Context&) {
  // 将更新委托给当前的行为策略
  if (current_behavior_) {
    current_behavior_->Update(delta_time, *this);
  } else {
    spdlog::warn("GameObject '{}' 上的 AIComponent 没有设置行为。",
                 owner_ ? owner_->GetName() : "Unknown");
  }
}

void AIComponent::setBehavior(std::unique_ptr<ai::AIBehavior> behavior) {
  current_behavior_ = std::move(behavior);
  spdlog::debug("GameObject '{}' 上的 AIComponent 设置了新的行为。",
                owner_ ? owner_->GetName() : "Unknown");
  if (current_behavior_) {
    current_behavior_->Enter(*this);  // 调用新行为的 enter 方法
  }
}

bool AIComponent::takeDamage(int damage) {
  bool success = false;
  if (auto* health_component =
          GetOwner()->GetComponent<engine::component::HealthComponent>();
      health_component) {
    success = health_component->TakeDamage(damage);
    // TODO: 可以设置受伤/死亡后的行为
  }
  return success;
}

bool AIComponent::isAlive() const {
  if (auto* health_component =
          GetOwner()->GetComponent<engine::component::HealthComponent>();
      health_component) {
    return health_component->IsAlive();
  }
  return true;  // 如果没有生命组件，默认返回存活状态
}

}  // namespace game::component
