// Copyright Sylar129

#pragma once

#include "engine/component/component.h"

namespace engine::component {

class HealthComponent final : public engine::component::Component {
  friend class engine::object::GameObject;

 public:
  explicit HealthComponent(int max_health = 1,
                           float invincibility_duration = 2.0f);
  ~HealthComponent() override = default;

  bool TakeDamage(int damage_amount);
  void Heal(int heal_amount);

  bool IsAlive() const { return current_health_ > 0; }
  bool IsInvincible() const { return is_invincible_; }
  int GetCurrentHealth() const { return current_health_; }
  int GetMaxHealth() const { return max_health_; }

  void SetCurrentHealth(int current_health);
  void SetMaxHealth(int max_health);
  void SetInvincible(float duration);
  void SetInvincibilityDuration(float duration) {
    invincibility_duration_ = duration;
  }

 protected:
  void Update(float, engine::core::Context&) override;

 private:
  int max_health_ = 1;
  int current_health_ = 1;
  bool is_invincible_ = false;
  float invincibility_duration_ = 2.0f;
  float invincibility_timer_ = 0.0f;
};

}  // namespace engine::component
