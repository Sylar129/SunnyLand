// Copyright Sylar129

#pragma once

#include "game/component/state/player_state.h"

namespace game::component::state {

class WalkState final : public PlayerState {
 public:
  WalkState(PlayerComponent* player_component)
      : PlayerState(player_component) {}
  ~WalkState() override = default;

 private:
  void Enter() override;
  void Exit() override;
  std::unique_ptr<PlayerState> HandleInput(engine::core::Context&) override;
  std::unique_ptr<PlayerState> Update(float delta_time,
                                      engine::core::Context&) override;
};

}  // namespace game::component::state
