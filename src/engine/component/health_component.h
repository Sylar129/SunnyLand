// Copyright Sylar129

#pragma once

namespace engine::component {

struct HealthComponent final {
  int max_health = 1;
  int current_health = 1;
  bool is_invincible = false;
  float invincibility_duration = 2.0f;
  float invincibility_timer = 0.0f;
};

}  // namespace engine::component
