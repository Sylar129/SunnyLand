// Copyright Sylar129

#include "engine/ui/state/ui_hover_state.h"

#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "engine/ui/state/ui_normal_state.h"
#include "engine/ui/state/ui_pressed_state.h"
#include "engine/ui/ui_interactive.h"
#include "utils/log.h"

namespace engine::ui::state {

void UIHoverState::Enter() {
  owner_->SetTexture("hover");
  ENGINE_LOG_DEBUG("Switched to hover state");
}

std::unique_ptr<UIState> UIHoverState::HandleInput(core::Context& context) {
  auto& input_manager = context.GetInputManager();
  auto mouse_pos = input_manager.GetLogicalMousePosition();
  if (!owner_->IsPointInside(mouse_pos)) {
    return std::make_unique<UINormalState>(owner_);
  }
  if (input_manager.IsActionPressed("MouseLeftClick")) {
    return std::make_unique<UIPressedState>(owner_);
  }
  return nullptr;
}

}  // namespace engine::ui::state
