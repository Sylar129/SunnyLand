// Copyright Sylar129

#include "game/component/state/climb_state.h"

#include "engine/component/animation_component.h"
#include "engine/component/physics_component.h"
#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "game/component/player_component.h"
#include "game/component/state/fall_state.h"
#include "game/component/state/idle_state.h"
#include "game/component/state/jump_state.h"
#include "log.h"

namespace game::component::state {

void ClimbState::Enter() {
  GAME_DEBUG("Enter ClimbState");
  PlayAnimation("climb");
  if (auto* physics = player_component_->GetPhysicsComponent(); physics) {
    physics->SetUseGravity(false);
  }
}

void ClimbState::Exit() {
  GAME_DEBUG("Exit ClimbState");
  if (auto* physics = player_component_->GetPhysicsComponent(); physics) {
    physics->SetUseGravity(true);
  }
}

std::unique_ptr<PlayerState> ClimbState::HandleInput(
    engine::core::Context& context) {
  auto input_manager = context.GetInputManager();
  auto physics_component = player_component_->GetPhysicsComponent();
  auto animation_component = player_component_->GetAnimationComponent();

  auto is_up = input_manager.IsActionDown("move_up");
  auto is_down = input_manager.IsActionDown("move_down");
  auto is_left = input_manager.IsActionDown("move_left");
  auto is_right = input_manager.IsActionDown("move_right");
  auto speed = player_component_->getClimbSpeed();

  physics_component->velocity_.y = is_up ? -speed : is_down ? speed : 0.0f;
  physics_component->velocity_.x = is_left ? -speed : is_right ? speed : 0.0f;

  (is_up || is_down || is_left || is_right)
      ? animation_component->resumeAnimation()
      : animation_component->StopAnimation();

  if (input_manager.IsActionPressed("jump")) {
    return std::make_unique<JumpState>(player_component_);
  }
  return nullptr;
}

std::unique_ptr<PlayerState> ClimbState::Update(float, engine::core::Context&) {
  auto physics_component = player_component_->GetPhysicsComponent();
  if (physics_component->HasCollidedBelow()) {
    return std::make_unique<IdleState>(player_component_);
  }
  if (!physics_component->HasCollidedLadder()) {
    return std::make_unique<FallState>(player_component_);
  }
  return nullptr;
}

}  // namespace game::component::state
