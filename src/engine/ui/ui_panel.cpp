// Copyright Sylar129

#include "engine/ui/ui_panel.h"

#include "engine/core/context.h"
#include "engine/render/renderer.h"
#include "utils/log.h"

namespace engine::ui {

UIPanel::UIPanel(const glm::vec2& position, const glm::vec2& size,
                 const std::optional<engine::utils::FColor>& background_color)
    : UIElement(position, size), background_color_(background_color) {
  ENGINE_LOG_TRACE("UIPanel created at position ({}, {}) with size ({}, {})",
                   position.x, position.y, size.x, size.y);
}

void UIPanel::Render(engine::core::Context& context) {
  if (!visible_) return;

  if (background_color_) {
    context.GetRenderer().DrawUIFilledRect(GetBounds(),
                                           background_color_.value());
  }

  UIElement::Render(context);
}

}  // namespace engine::ui
