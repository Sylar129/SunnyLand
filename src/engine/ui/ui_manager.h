// Copyright Sylar129

#pragma once

#include <memory>

#include "glm/vec2.hpp"
#include "utils/non_copyable.h"

namespace engine::core {
class Context;
}
namespace engine::ui {
class UIElement;
class UIPanel;
}  // namespace engine::ui

namespace engine::ui {

class UIManager final {
 public:
  UIManager();
  ~UIManager();

  DISABLE_COPY_AND_MOVE(UIManager);

  [[nodiscard]] bool Init(const glm::vec2& window_size);
  void AddElement(std::unique_ptr<UIElement> element);
  UIPanel* GetRootElement() const;
  void ClearElements();

  bool HandleInput(core::Context&);
  void Update(float delta_time, core::Context&);
  void Render(core::Context&);

 private:
  std::unique_ptr<UIPanel> root_element_;
};

}  // namespace engine::ui
