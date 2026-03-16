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

  [[nodiscard]] bool init(const glm::vec2& window_size);
  void addElement(std::unique_ptr<UIElement> element);
  UIPanel* getRootElement() const;
  void clearElements();

  bool handleInput(engine::core::Context&);
  void update(float delta_time, engine::core::Context&);
  void render(engine::core::Context&);

 private:
  std::unique_ptr<UIPanel> root_element_;
};

}  // namespace engine::ui
