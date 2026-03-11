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

  if (!transform_component_ || !physics_component_ || !sprite_component_ ||
      !animation_component_ || !health_component_) {
    GAME_ERROR("Player missing required components!");
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

bool PlayerComponent::takeDamage(int damage) {
  if (is_dead_ || !health_component_ || damage <= 0) {
    GAME_WARN("玩家已死亡或却少必要组件，并未造成伤害。");
    return false;
  }

  bool success = health_component_->takeDamage(damage);
  if (!success) return false;
  // --- 成功造成伤害了，根据是否存活决定状态切换
  if (health_component_->isAlive()) {
    GAME_DEBUG("玩家受到了 {} 点伤害，当前生命值: {}/{}。", damage,
               health_component_->getCurrentHealth(),
               health_component_->getMaxHealth());
    // 切换到受伤状态
    SetState(std::make_unique<state::HurtState>(this));
  } else {
    GAME_DEBUG("玩家死亡。");
    is_dead_ = true;
    // 切换到死亡状态
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
