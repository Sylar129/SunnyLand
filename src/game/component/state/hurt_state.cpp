// Copyright Sylar129

#include "game/component/state/hurt_state.h"

#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "game/component/player_component.h"
#include "game/component/state/fall_state.h"
#include "game/component/state/idle_state.h"

namespace game::component::state {

void HurtState::Enter() {
  PlayAnimation("hurt");

  auto physics_component = player_component_->GetPhysicsComponent();
  auto sprite_component = player_component_->GetSpriteComponent();
  auto knockback_velocity = glm::vec2(-100.0f, -150.0f);
  if (sprite_component->IsFlipped()) {
    knockback_velocity.x = -knockback_velocity.x;
  }
  physics_component->velocity_ = knockback_velocity;
}

void HurtState::Exit() {}

std::unique_ptr<PlayerState> HurtState::HandleInput(engine::core::Context&) {
  return nullptr;
}

std::unique_ptr<PlayerState> HurtState::Update(float delta_time,
                                               engine::core::Context&) {
  stunned_timer_ += delta_time;
  auto physics_component = player_component_->GetPhysicsComponent();
  if (physics_component->HasCollidedBelow()) {
    return std::make_unique<IdleState>(player_component_);
  }
  if (stunned_timer_ > player_component_->getStunnedDuration()) {
    return std::make_unique<FallState>(player_component_);
  }
  return nullptr;
}
}  // namespace game::component::state
