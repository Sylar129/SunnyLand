// Copyright Sylar129

#include "game/component/player_component.h"

#include <utility>

#include "engine/component/animation_component.h"
#include "engine/component/health_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/transform_component.h"
#include "engine/object/game_object.h"
#include "engine/utils/assert.h"
#include "game/component/state/dead_state.h"
#include "game/component/state/hurt_state.h"
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
  animation_component_ =
      owner_->GetComponent<engine::component::AnimationComponent>();
  health_component_ =
      owner_->GetComponent<engine::component::HealthComponent>();

  GAME_ASSERT(transform_component_,
              "PlayerComponent requires TransformComponent");
  GAME_ASSERT(physics_component_, "PlayerComponent requires PhysicsComponent");
  GAME_ASSERT(sprite_component_, "PlayerComponent requires SpriteComponent");
  GAME_ASSERT(animation_component_,
              "PlayerComponent requires AnimationComponent");
  GAME_ASSERT(health_component_, "PlayerComponent requires HealthComponent");

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

bool PlayerComponent::TakeDamage(int damage) {
  if (is_dead_ || damage <= 0) {
    GAME_WARN("Player cannot take damage. is_dead: {},  damage: {}", is_dead_,
              damage);
    return false;
  }

  bool success = health_component_->TakeDamage(damage);
  if (!success) return false;
  if (health_component_->IsAlive()) {
    SetState(std::make_unique<state::HurtState>(this));
  } else {
    is_dead_ = true;
    SetState(std::make_unique<state::DeadState>(this));
  }
  return true;
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
