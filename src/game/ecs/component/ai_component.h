// Copyright Sylar129

#pragma once

#include "game/ecs/component/ai_behavior_type.h"

namespace game::ecs {

struct AiComponent {
  AiBehaviorType behavior = AiBehaviorType::kNone;
  float min_bound = 0.0f;
  float max_bound = 0.0f;
  float speed = 50.0f;
  float jump_interval = 2.0f;
  float jump_timer = 0.0f;
};

}  // namespace game::ecs