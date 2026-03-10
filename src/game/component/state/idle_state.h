// Copyright Sylar129

#pragma once

#include "game/component/state/player_state.h"

namespace game::component::state {

class IdleState final : public PlayerState {
 public:
  IdleState(PlayerComponent* player_component)
      : PlayerState(player_component) {}
  ~IdleState() override = default;

 private:
  void Enter() override;
  void Exit() override;
  std::unique_ptr<PlayerState> HandleInput(engine::core::Context&) override;
  std::unique_ptr<PlayerState> Update(float delta_time,
                                      engine::core::Context&) override;
};

}  // namespace game::component::state
