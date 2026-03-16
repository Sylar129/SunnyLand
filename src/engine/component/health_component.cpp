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

bool HealthComponent::TakeDamage(int damage_amount) {
  if (damage_amount <= 0 || !IsAlive()) {
    return false;
  }

  if (is_invincible_) {
    ENGINE_DEBUG(
        "GameObject '{}' is currently invincible and cannot take damage.",
        owner_->GetName());
    return false;
  }
  current_health_ -= damage_amount;
  current_health_ = glm::max(0, current_health_);
  if (IsAlive() && invincibility_duration_ > 0.0f) {
    SetInvincible(invincibility_duration_);
  }
  ENGINE_DEBUG("GameObject '{}' took {} damage, current health: {}/{}.",
               owner_->GetName(), damage_amount, current_health_, max_health_);
  return true;
}

int HealthComponent::Heal(int heal_amount) {
  if (heal_amount <= 0 || !IsAlive()) {
    return current_health_;
  }

  current_health_ += heal_amount;
  current_health_ = std::min(max_health_, current_health_);
  ENGINE_DEBUG("GameObject '{}' healed {} health, current health: {}/{}.",
               owner_->GetName(), heal_amount, current_health_, max_health_);
  return current_health_;
}

void HealthComponent::SetInvincible(float duration) {
  if (duration > 0.0f) {
    is_invincible_ = true;
    invincibility_timer_ = duration;
    ENGINE_DEBUG("GameObject '{}' is now invincible for {} seconds.",
                 owner_->GetName(), duration);
  } else {
    is_invincible_ = false;
    invincibility_timer_ = 0.0f;
    ENGINE_DEBUG("GameObject '{}' is no longer invincible.", owner_->GetName());
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
