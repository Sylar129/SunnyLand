// Copyright Sylar129

#include "game/component/state/idle_state.h"

#include "engine/component/physics_component.h"
#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "game/component/player_component.h"
#include "game/component/state/climb_state.h"
#include "game/component/state/fall_state.h"
#include "game/component/state/jump_state.h"
#include "game/component/state/walk_state.h"

namespace game::component::state {

void IdleState::Enter() { PlayAnimation("idle"); }

void IdleState::Exit() {}

std::unique_ptr<PlayerState> IdleState::HandleInput(
    engine::core::Context& context) {
  auto input_manager = context.GetInputManager();

  auto physics_component = player_component_->GetPhysicsComponent();
  if (physics_component->HasCollidedLadder() &&
      input_manager.IsActionDown("move_up")) {
    return std::make_unique<ClimbState>(player_component_);
  }

  if (physics_component->IsOnTopLadder() &&
      input_manager.IsActionDown("move_down")) {
    player_component_->GetTransformComponent()->Translate(
        glm::vec2(0, 2.0f));  // A triky way
    return std::make_unique<ClimbState>(player_component_);
  }

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
  physics_component->SetVelocityX(physics_component->GetVelocity().x *
                                  friction_factor);

  if (!player_component_->IsOnGround()) {
    return std::make_unique<FallState>(player_component_);
  }
  return nullptr;
}

}  // namespace game::component::state
