// Copyright Sylar129

#include "engine/component/health_component.h"

#include "engine/object/game_object.h"
#include "glm/common.hpp"
#include "utils/log.h"

namespace engine::component {

HealthComponent::HealthComponent(int max_health, float invincibility_duration)
    : max_health_(glm::max(1, max_health)),
      current_health_(max_health_),
      invincibility_duration_(invincibility_duration) {}

void HealthComponent::Update(float delta_time, core::Context&) {
  if (is_invincible_) {
    invincibility_timer_ -= delta_time;
    if (invincibility_timer_ <= 0.0f) {
      is_invincible_ = false;
      invincibility_timer_ = 0.0f;
    }
  }
}

bool HealthComponent::TakeDamage(int damage_amount) {
  if (damage_amount <= 0 || !IsAlive()) {
    return false;
  }

  if (is_invincible_) {
    return false;
  }
  current_health_ -= damage_amount;
  current_health_ = glm::max(0, current_health_);
  if (IsAlive() && invincibility_duration_ > 0.0f) {
    SetInvincible(invincibility_duration_);
  }
  return true;
}

int HealthComponent::Heal(int heal_amount) {
  if (heal_amount <= 0 || !IsAlive()) {
    return current_health_;
  }

  current_health_ += heal_amount;
  current_health_ = std::min(max_health_, current_health_);
  return current_health_;
}

void HealthComponent::SetInvincible(float duration) {
  if (duration > 0.0f) {
    is_invincible_ = true;
    invincibility_timer_ = duration;
  } else {
    is_invincible_ = false;
    invincibility_timer_ = 0.0f;
  }
}

void HealthComponent::SetMaxHealth(int max_health) {
  max_health_ = glm::max(1, max_health);
  current_health_ = glm::min(current_health_, max_health_);
}

void HealthComponent::SetCurrentHealth(int current_health) {
  current_health_ = glm::max(0, glm::min(current_health, max_health_));
}

}  // namespace engine::component
