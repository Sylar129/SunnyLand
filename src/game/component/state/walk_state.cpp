// Copyright Sylar129

#include "game/component/state/walk_state.h"

#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "game/component/player_component.h"
#include "game/component/state/climb_state.h"
#include "game/component/state/fall_state.h"
#include "game/component/state/idle_state.h"
#include "game/component/state/jump_state.h"
#include "glm/common.hpp"

namespace game::component::state {

void WalkState::Enter() { PlayAnimation("walk"); }

void WalkState::Exit() {}

std::unique_ptr<PlayerState> WalkState::HandleInput(
    engine::core::Context& context) {
  auto input_manager = context.GetInputManager();
  auto physics_component = player_component_->GetPhysicsComponent();
  auto sprite_component = player_component_->GetSpriteComponent();

  if (input_manager.IsActionPressed("jump")) {
    return std::make_unique<JumpState>(player_component_);
  }

  if (input_manager.IsActionDown("move_left")) {
    if (physics_component->GetVelocity().x > 0.0f) {
      physics_component->SetVelocityX(0.0f);
    }
    physics_component->AddForce({-player_component_->GetMoveForce(), 0.0f});
    sprite_component->SetFlipped(true);
  } else if (input_manager.IsActionDown("move_right")) {
    if (physics_component->GetVelocity().x < 0.0f) {
      physics_component->SetVelocityX(0.0f);
    }
    physics_component->AddForce({player_component_->GetMoveForce(), 0.0f});
    sprite_component->SetFlipped(false);
  } else {
    return std::make_unique<IdleState>(player_component_);
  }

  if (physics_component->HasCollidedLadder() &&
      input_manager.IsActionDown("move_up")) {
    return std::make_unique<ClimbState>(player_component_);
  }

  return nullptr;
}

std::unique_ptr<PlayerState> WalkState::Update(float, engine::core::Context&) {
  auto physics_component = player_component_->GetPhysicsComponent();
  auto max_speed = player_component_->GetMaxSpeed();
  physics_component->SetVelocityX(
      glm::clamp(physics_component->GetVelocity().x, -max_speed, max_speed));

  if (!player_component_->IsOnGround()) {
    return std::make_unique<FallState>(player_component_);
  }

  return nullptr;
}

}  // namespace game::component::state
