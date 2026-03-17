// Copyright Sylar129

#include "engine/ui/state/ui_pressed_state.h"

#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "engine/ui/state/ui_hover_state.h"
#include "engine/ui/state/ui_normal_state.h"
#include "engine/ui/ui_interactive.h"
#include "utils/log.h"

namespace engine::ui::state {

void UIPressedState::Enter() {
  owner_->SetSprite("pressed");
  owner_->PlaySound("pressed");
  ENGINE_LOG_DEBUG("Switched to pressed state");
}

std::unique_ptr<UIState> UIPressedState::HandleInput(
    engine::core::Context& context) {
  auto& input_manager = context.GetInputManager();
  auto mouse_pos = input_manager.GetLogicalMousePosition();
  if (input_manager.IsActionReleased("MouseLeftClick")) {
    if (!owner_->IsPointInside(mouse_pos)) {
      return std::make_unique<engine::ui::state::UINormalState>(owner_);
    } else {
      owner_->Clicked();
      return std::make_unique<engine::ui::state::UIHoverState>(owner_);
    }
  }

  return nullptr;
}

}  // namespace engine::ui::state
