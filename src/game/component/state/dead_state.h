#pragma once
#include "player_state.h"

namespace game::component::state {

class DeadState final : public PlayerState {
  friend class game::component::PlayerComponent;

 public:
  DeadState(PlayerComponent* player_component)
      : PlayerState(player_component) {}
  ~DeadState() override = default;

 private:
  void Enter() override;
  void Exit() override;
  std::unique_ptr<PlayerState> HandleInput(engine::core::Context&) override;
  std::unique_ptr<PlayerState> Update(float delta_time,
                                      engine::core::Context&) override;
};

}  // namespace game::component::state
