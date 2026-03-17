// Copyright Sylar129

#pragma once

#include <memory>

#include "utils/non_copyable.h"

namespace engine::core {
class Context;
}

namespace game::component {
class PlayerComponent;
}

namespace game::component::state {

class PlayerState {
  friend class game::component::PlayerComponent;

 public:
  PlayerState(PlayerComponent* player_component)
      : player_component_(player_component) {}
  virtual ~PlayerState() = default;

  DISABLE_COPY_AND_MOVE(PlayerState);

  void PlayAnimation(const std::string& animation_name);

 protected:
  virtual void Enter() = 0;
  virtual void Exit() = 0;
  virtual std::unique_ptr<PlayerState> HandleInput(engine::core::Context&) = 0;
  virtual std::unique_ptr<PlayerState> Update(float,
                                              engine::core::Context&) = 0;

  PlayerComponent* player_component_ = nullptr;
};

}  // namespace game::component::state
