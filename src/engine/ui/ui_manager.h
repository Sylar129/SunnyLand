// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/utils/non_copyable.h"
#include "glm/vec2.hpp"

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

  bool HandleInput(engine::core::Context&);
  void Update(float delta_time, engine::core::Context&);
  void Render(engine::core::Context&);

 private:
  std::unique_ptr<UIPanel> root_element_;
};

}  // namespace engine::ui
