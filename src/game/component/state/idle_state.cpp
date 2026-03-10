// Copyright Sylar129

#include "game/component/state/idle_state.h"

#include "engine/component/physics_component.h"
#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "game/component/player_component.h"
#include "game/component/state/fall_state.h"
#include "game/component/state/jump_state.h"
#include "game/component/state/walk_state.h"

namespace game::component::state {

void IdleState::Enter() { playAnimation("idle"); }

void IdleState::Exit() {}

std::unique_ptr<PlayerState> IdleState::HandleInput(
    engine::core::Context& context) {
  auto input_manager = context.GetInputManager();
  if (input_manager.IsActionDown("move_left") ||
      input_manager.IsActionDown("move_right")) {
    return std::make_unique<WalkState>(player_component_);
  }

  if (input_manager.IsActionPressed("jump")) {
    return std::make_unique<JumpState>(player_component_);
  }
  return nullptr;
}

std::unique_ptr<PlayerState> IdleState::Update(float, engine::core::Context&) {
  auto physics_component = player_component_->GetPhysicsComponent();
  auto friction_factor = player_component_->SetFrictionFactor();
  physics_component->velocity_.x *= friction_factor;

  if (!physics_component->HasCollidedBelow()) {
    return std::make_unique<FallState>(player_component_);
  }
  return nullptr;
}

}  // namespace game::component::state
