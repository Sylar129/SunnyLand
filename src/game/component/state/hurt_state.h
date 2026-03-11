#pragma once
#include "player_state.h"

namespace game::component::state {

class HurtState final : public PlayerState {
  friend class game::component::PlayerComponent;

 private:
  float stunned_timer_ = 0.0f;  ///< @brief 硬直计时器，单位为秒

 public:
  HurtState(PlayerComponent* player_component)
      : PlayerState(player_component) {}
  // ...
 private:
  void Enter() override;
  void Exit() override;
  std::unique_ptr<PlayerState> HandleInput(engine::core::Context&) override;
  std::unique_ptr<PlayerState> Update(float delta_time,
                                      engine::core::Context&) override;
};

}  // namespace game::component::state
