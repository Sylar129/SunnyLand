// Copyright Sylar129

#pragma once

#include "game/component/state/player_state.h"

namespace game::component::state {

class HurtState final : public PlayerState {
 public:
  HurtState(PlayerComponent* player_component)
      : PlayerState(player_component) {}
  ~HurtState() override = default;

 private:
  void Enter() override;
  void Exit() override;
  std::unique_ptr<PlayerState> HandleInput(engine::core::Context&) override;
  std::unique_ptr<PlayerState> Update(float delta_time,
                                      engine::core::Context&) override;

  float stunned_timer_ = 0.0f;  // In seconds
};

}  // namespace game::component::state
