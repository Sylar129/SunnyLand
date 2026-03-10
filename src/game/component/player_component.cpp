// Copyright Sylar129

#include "game/component/player_component.h"

#include <utility>

#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/transform_component.h"
#include "engine/object/game_object.h"
#include "engine/utils/assert.h"
#include "game/component/state/idle_state.h"
#include "log.h"

namespace game::component {

void PlayerComponent::Init() {
  GAME_ASSERT(owner_, "player component must have an owner game object");

  transform_component_ =
      owner_->GetComponent<engine::component::TransformComponent>();
  physics_component_ =
      owner_->GetComponent<engine::component::PhysicsComponent>();
  sprite_component_ =
      owner_->GetComponent<engine::component::SpriteComponent>();

  if (!transform_component_ || !physics_component_ || !sprite_component_) {
    GAME_ERROR(
        "Player missing required components! Transform: {}, Physics: {}, "
        "Sprite: {}",
        transform_component_ != nullptr, physics_component_ != nullptr,
        sprite_component_ != nullptr);
  }

  SetState(std::make_unique<state::IdleState>(this));

  GAME_DEBUG("Init PlayerComponent successfully.");
}

void PlayerComponent::SetState(std::unique_ptr<state::PlayerState> new_state) {
  if (current_state_) {
    current_state_->Exit();
  }

  current_state_ = std::move(new_state);
  current_state_->Enter();
}

void PlayerComponent::HandleInput(engine::core::Context& context) {
  if (!current_state_) return;

  auto next_state = current_state_->HandleInput(context);
  if (next_state) {
    SetState(std::move(next_state));
  }
}

void PlayerComponent::Update(float delta_time, engine::core::Context& context) {
  if (!current_state_) return;

  auto next_state = current_state_->Update(delta_time, context);
  if (next_state) {
    SetState(std::move(next_state));
  }
}

}  // namespace game::component
