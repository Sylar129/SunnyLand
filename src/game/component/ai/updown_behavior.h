// Copyright Sylar129

#pragma once

#include "game/component/ai/ai_behavior.h"

namespace game::component::ai {

class UpDownBehavior final : public AIBehavior {
  friend class game::component::AIComponent;

 public:
  UpDownBehavior(float min_y, float max_y, float speed = 50.0f);
  ~UpDownBehavior() override = default;

 private:
  void Enter(AIComponent& ai_component) override;
  void Update(float delta_time, AIComponent& ai_component) override;

  float patrol_min_y_ = 0.0f;
  float patrol_max_y_ = 0.0f;
  float move_speed_ = 50.0f;
  bool moving_down_ = false;
};

}  // namespace game::component::ai
