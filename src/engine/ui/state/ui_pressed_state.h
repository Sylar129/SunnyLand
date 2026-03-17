// Copyright Sylar129

#pragma once

#include "engine/ui/state/ui_state.h"

namespace engine::ui::state {

class UIPressedState final : public UIState {
 public:
  UIPressedState(engine::ui::UIInteractive* owner) : UIState(owner) {}
  ~UIPressedState() override = default;

 private:
  void Enter() override;
  std::unique_ptr<UIState> HandleInput(engine::core::Context& context) override;
};

}  // namespace engine::ui::state
