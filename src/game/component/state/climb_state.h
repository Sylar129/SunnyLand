// Copyright Sylar129

#pragma once

#include "game/component/state/player_state.h"

namespace game::component::state {

class ClimbState final : public PlayerState {
 public:
  ClimbState(PlayerComponent* player_component)
      : PlayerState(player_component) {}
  ~ClimbState() override = default;

 private:
  void Enter() override;
  void Exit() override;
  std::unique_ptr<PlayerState> HandleInput(engine::core::Context&) override;
  std::unique_ptr<PlayerState> Update(float delta_time,
                                      engine::core::Context&) override;
};

}  // namespace game::component::state
