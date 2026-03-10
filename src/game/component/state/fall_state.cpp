// Copyright Sylar129

#include "game/component/state/fall_state.h"

#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "game/component/player_component.h"
#include "game/component/state/idle_state.h"
#include "game/component/state/walk_state.h"
#include "glm/common.hpp"

namespace game::component::state {

void FallState::Enter() { playAnimation("fall"); }

void FallState::Exit() {}

std::unique_ptr<PlayerState> FallState::HandleInput(
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

std::unique_ptr<PlayerState> FallState::Update(float, engine::core::Context&) {
  auto physics_component = player_component_->GetPhysicsComponent();
  auto max_speed = player_component_->GetMaxSpeed();
  physics_component->velocity_.x =
      glm::clamp(physics_component->velocity_.x, -max_speed, max_speed);

  if (physics_component->HasCollidedBelow()) {
    if (glm::abs(physics_component->velocity_.x) < 1.0f) {
      return std::make_unique<IdleState>(player_component_);
    } else {
      return std::make_unique<WalkState>(player_component_);
    }
  }
  return nullptr;
}

}  // namespace game::component::state
