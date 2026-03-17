// Copyright Sylar129

#include "game/component/state/dead_state.h"

#include "engine/component/collider_component.h"
#include "engine/component/physics_component.h"
#include "engine/object/game_object.h"
#include "game/component/player_component.h"
#include "utils/log.h"

namespace game::component::state {

void DeadState::Enter() {
  GAME_LOG_DEBUG("Player entering DeadState.");
  PlayAnimation("hurt");

  auto physics_component = player_component_->GetPhysicsComponent();
  physics_component->SetVelocity({0.0f, -200.0f});

  auto collider_component =
      player_component_->GetOwner()
          ->GetComponent<engine::component::ColliderComponent>();
  if (collider_component) {
    collider_component->SetActive(false);
  }
}

void DeadState::Exit() {}

std::unique_ptr<PlayerState> DeadState::HandleInput(engine::core::Context&) {
  return nullptr;
}

std::unique_ptr<PlayerState> DeadState::Update(float, engine::core::Context&) {
  return nullptr;
}
}  // namespace game::component::state
