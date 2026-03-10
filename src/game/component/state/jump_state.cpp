// Copyright Sylar129

#include "game/component/state/jump_state.h"

#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "game/component/player_component.h"
#include "game/component/state/fall_state.h"
#include "glm/common.hpp"
#include "log.h"

namespace game::component::state {

void JumpState::Enter() {
  auto physics_component = player_component_->GetPhysicsComponent();
  physics_component->velocity_.y = -player_component_->GetJumpForce();
  GAME_DEBUG("PlayerComponent entered JumpState with initial jump velocity: {}",
             physics_component->velocity_.y);
}

void JumpState::Exit() {}

std::unique_ptr<PlayerState> JumpState::HandleInput(
    engine::core::Context& context) {
  auto input_manager = context.GetInputManager();
  auto physics_component = player_component_->GetPhysicsComponent();
  auto sprite_component = player_component_->GetSpriteComponent();

  if (input_manager.IsActionDown("move_left")) {
    if (physics_component->velocity_.x > 0.0f)
      physics_component->velocity_.x = 0.0f;
    physics_component->AddForce({-player_component_->GetMoveForce(), 0.0f});
    sprite_component->SetFlipped(true);
  } else if (input_manager.IsActionDown("move_right")) {
    if (physics_component->velocity_.x < 0.0f)
      physics_component->velocity_.x = 0.0f;
    physics_component->AddForce({player_component_->GetMoveForce(), 0.0f});
    sprite_component->SetFlipped(false);
  }
  return nullptr;
}

std::unique_ptr<PlayerState> JumpState::Update(float, engine::core::Context&) {
  auto physics_component = player_component_->GetPhysicsComponent();
  auto max_speed = player_component_->GetMaxSpeed();
  physics_component->velocity_.x =
      glm::clamp(physics_component->velocity_.x, -max_speed, max_speed);

  if (physics_component->velocity_.y > 0.0f) {
    return std::make_unique<FallState>(player_component_);
  }

  return nullptr;
}

}  // namespace game::component::state
