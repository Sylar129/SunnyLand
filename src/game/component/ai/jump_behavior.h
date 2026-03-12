// Copyright Sylar129

#pragma once

#include "game/component/ai/ai_behavior.h"
#include "glm/vec2.hpp"

namespace game::component::ai {

class JumpBehavior final : public AIBehavior {
  friend class game::component::AIComponent;

 public:
  JumpBehavior(float min_x, float max_x,
               const glm::vec2& jump_vel = glm::vec2(100.0f, -300.0f),
               float jump_interval = 2.0f);
  ~JumpBehavior() override = default;

 private:
  void Update(float delta_time, AIComponent& ai_component) override;

  float patrol_min_x_ = 0.0f;
  float patrol_max_x_ = 0.0f;
  glm::vec2 jump_vel_ = glm::vec2(100.0f, -300.0f);
  float jump_interval_ = 2.0f;
  float jump_timer_ = 0.0f;
  bool jumping_right_ = false;
};

}  // namespace game::component::ai
