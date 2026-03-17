// Copyright Sylar129

#include "game/component/player_component.h"

#include <utility>

#include "engine/component/animation_component.h"
#include "engine/component/health_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/transform_component.h"
#include "engine/object/game_object.h"
#include "game/component/state/dead_state.h"
#include "game/component/state/hurt_state.h"
#include "game/component/state/idle_state.h"
#include "utils/assert.h"
#include "utils/log.h"

namespace game::component {

void PlayerComponent::Init() {
  GAME_LOG_ASSERT(owner_, "player component must have an owner game object");

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

  GAME_LOG_ASSERT(transform_component_,
                  "PlayerComponent requires TransformComponent");
  GAME_LOG_ASSERT(physics_component_,
                  "PlayerComponent requires PhysicsComponent");
  GAME_LOG_ASSERT(sprite_component_,
                  "PlayerComponent requires SpriteComponent");
  GAME_LOG_ASSERT(animation_component_,
                  "PlayerComponent requires AnimationComponent");
  GAME_LOG_ASSERT(health_component_,
                  "PlayerComponent requires HealthComponent");

  SetState(std::make_unique<state::IdleState>(this));

  GAME_LOG_DEBUG("Init PlayerComponent successfully.");
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
    GAME_LOG_WARN("Player cannot take damage. is_dead: {},  damage: {}",
                  is_dead_, damage);
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

bool PlayerComponent::IsOnGround() const {
  return coyote_timer_ <= coyote_time_ ||
         physics_component_->HasCollidedBelow();
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

  if (!physics_component_->HasCollidedBelow()) {
    coyote_timer_ += delta_time;
  } else {
    coyote_timer_ = 0.0f;
  }

  if (health_component_->IsInvincible()) {
    flash_timer_ += delta_time;
    if (flash_timer_ >= 2 * flash_interval_) {
      flash_timer_ -= 2 * flash_interval_;
    }
    if (flash_timer_ < flash_interval_) {
      sprite_component_->SetHidden(true);
    } else {
      sprite_component_->SetHidden(false);
    }
  } else if (sprite_component_->IsHidden()) {
    sprite_component_->SetHidden(false);
  }

  auto next_state = current_state_->Update(delta_time, context);
  if (next_state) {
    SetState(std::move(next_state));
  }
}

}  // namespace game::component
