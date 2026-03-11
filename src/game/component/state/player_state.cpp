// Copyright Sylar129

#include "game/component/state/player_state.h"

#include "engine/component/animation_component.h"
#include "game/component/player_component.h"

namespace game::component::state {

void PlayerState::PlayAnimation(const std::string& animation_name) {
  if (!player_component_) {
    return;
  }

  auto animation_component = player_component_->GetAnimationComponent();
  if (!animation_component) {
    return;
  }

  animation_component->PlayAnimation(animation_name);
}

}  // namespace game::component::state
