// Copyright Sylar129

#pragma once

#include "game/ecs/component/player_state.h"

namespace game::ecs {

struct PlayerComponent {
  PlayerState state = PlayerState::kIdle;
  bool is_dead = false;
  float move_force = 200.0f;
  float max_speed = 120.0f;
  float climb_speed = 100.0f;
  float friction_factor = 0.85f;
  float jump_velocity = 350.0f;
  float stunned_duration = 0.4f;
  float coyote_time = 0.1f;
  float coyote_timer = 0.0f;
  float flash_interval = 0.1f;
  float flash_timer = 0.0f;
  float state_timer = 0.0f;
};

}  // namespace game::ecs