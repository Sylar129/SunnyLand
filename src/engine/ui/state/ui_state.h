// Copyright Sylar129

#pragma once

#include <memory>

#include "utils/non_copyable.h"

namespace engine::core {
class Context;
}

namespace engine::ui {
class UIInteractive;
}

namespace engine::ui::state {

class UIState {
  friend class engine::ui::UIInteractive;

 public:
  UIState(engine::ui::UIInteractive* owner) : owner_(owner) {}
  virtual ~UIState() = default;

  DISABLE_COPY_AND_MOVE(UIState);

 protected:
  virtual void Enter() {}
  virtual std::unique_ptr<UIState> HandleInput(
      engine::core::Context& context) = 0;

  engine::ui::UIInteractive* owner_ = nullptr;
};

}  // namespace engine::ui::state
