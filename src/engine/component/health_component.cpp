// Copyright Sylar129

#include "engine/component/health_component.h"

#include "engine/object/game_object.h"
#include "glm/common.hpp"
#include "log.h"

namespace engine::component {

HealthComponent::HealthComponent(int max_health, float invincibility_duration)
    : max_health_(glm::max(1, max_health)),
      current_health_(max_health_),
      invincibility_duration_(invincibility_duration) {}

void HealthComponent::Update(float delta_time, engine::core::Context&) {
  if (is_invincible_) {
    invincibility_timer_ -= delta_time;
    if (invincibility_timer_ <= 0.0f) {
      is_invincible_ = false;
      invincibility_timer_ = 0.0f;
    }
  }
}

bool HealthComponent::takeDamage(int damage_amount) {
  if (damage_amount <= 0 || !isAlive()) {
    return false;
  }

  if (is_invincible_) {
    ENGINE_DEBUG("游戏对象 '{}' 处于无敌状态，免疫了 {} 点伤害。",
                  owner_ ? owner_->GetName() : "Unknown", damage_amount);
    return false;  // 无敌状态，不受伤
  }
  current_health_ -= damage_amount;
  current_health_ = glm::max(0, current_health_);
  if (isAlive() && invincibility_duration_ > 0.0f) {
    setInvincible(invincibility_duration_);
  }
  ENGINE_DEBUG("游戏对象 '{}' 受到了 {} 点伤害，当前生命值: {}/{}。",
               owner_ ? owner_->GetName() : "Unknown", damage_amount,
               current_health_, max_health_);
  return true;
}

void HealthComponent::heal(int heal_amount) {
  if (heal_amount <= 0 || !isAlive()) {
    return;
  }

  current_health_ += heal_amount;
  current_health_ = std::min(max_health_, current_health_);
  ENGINE_DEBUG("游戏对象 '{}' 治疗了 {} 点，当前生命值: {}/{}。",
               owner_ ? owner_->GetName() : "Unknown", heal_amount,
               current_health_, max_health_);
}

void HealthComponent::setInvincible(float duration) {
  if (duration > 0.0f) {
    is_invincible_ = true;
    invincibility_timer_ = duration;
    ENGINE_DEBUG("游戏对象 '{}' 进入无敌状态，持续 {} 秒。",
                 owner_ ? owner_->GetName() : "Unknown", duration);
  } else {
    is_invincible_ = false;
    invincibility_timer_ = 0.0f;
    ENGINE_DEBUG("游戏对象 '{}' 的无敌状态被手动移除。",
                 owner_ ? owner_->GetName() : "Unknown");
  }
}

void HealthComponent::setMaxHealth(int max_health) {
  max_health_ = glm::max(1, max_health);
  current_health_ = glm::min(current_health_, max_health_);
}

void HealthComponent::setCurrentHealth(int current_health) {
  current_health_ = glm::max(0, glm::min(current_health, max_health_));
}

}  // namespace engine::component
