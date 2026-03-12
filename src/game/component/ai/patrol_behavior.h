// Copyright Sylar129

#pragma once

#include "game/component/ai/ai_behavior.h"

namespace game::component::ai {

class PatrolBehavior final : public AIBehavior {
  friend class game::component::AIComponent;

 public:
  PatrolBehavior(float min_x, float max_x, float speed = 50.0f);
  ~PatrolBehavior() override = default;

 private:
  void Enter(AIComponent& ai_component) override;
  void Update(float delta_time, AIComponent& ai_component) override;

  float patrol_min_x_ = 0.0f;
  float patrol_max_x_ = 0.0f;
  float move_speed_ = 50.0f;
  bool moving_right_ = false;
};

}  // namespace game::component::ai
