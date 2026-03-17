// Copyright Sylar129

#include "engine/ui/state/ui_normal_state.h"

#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "engine/ui/state/ui_hover_state.h"
#include "engine/ui/ui_interactive.h"
#include "log.h"

namespace engine::ui::state {

void UINormalState::Enter() {
  owner_->SetSprite("normal");
  ENGINE_DEBUG("Switched to normal state");
}

std::unique_ptr<UIState> UINormalState::HandleInput(
    engine::core::Context& context) {
  auto& input_manager = context.GetInputManager();
  auto mouse_pos = input_manager.GetLogicalMousePosition();
  if (owner_->IsPointInside(mouse_pos)) {
    owner_->PlaySound("hover");
    return std::make_unique<engine::ui::state::UIHoverState>(owner_);
  }
  return nullptr;
}

}  // namespace engine::ui::state
