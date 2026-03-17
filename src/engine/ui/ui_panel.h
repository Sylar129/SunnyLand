// Copyright Sylar129

#pragma once

#include <optional>

#include "engine/ui/ui_element.h"
#include "utils/math.h"

namespace engine::ui {

class UIPanel final : public UIElement {
 public:
  explicit UIPanel(
      const glm::vec2& position = {0.0f, 0.0f},
      const glm::vec2& size = {0.0f, 0.0f},
      const std::optional<utils::FColor>& background_color = std::nullopt);

  void SetBackgroundColor(
      const std::optional<utils::FColor>& background_color) {
    background_color_ = background_color;
  }
  const std::optional<utils::FColor>& GetBackgroundColor() const {
    return background_color_;
  }

  void Render(engine::core::Context& context) override;

 private:
  std::optional<utils::FColor> background_color_;
};

}  // namespace engine::ui
