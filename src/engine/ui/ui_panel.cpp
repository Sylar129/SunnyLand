// Copyright Sylar129

#include "engine/ui/ui_panel.h"

#include "engine/core/context.h"
#include "engine/render/renderer.h"
#include "log.h"

namespace engine::ui {

UIPanel::UIPanel(const glm::vec2& position, const glm::vec2& size,
                 const std::optional<engine::utils::FColor>& background_color)
    : UIElement(position, size), background_color_(background_color) {
  ENGINE_TRACE("UIPanel created at position ({}, {}) with size ({}, {})",
               position.x, position.y, size.x, size.y);
}

void UIPanel::render(engine::core::Context& context) {
  if (!visible_) return;

  if (background_color_) {
    context.GetRenderer().drawUIFilledRect(getBounds(),
                                           background_color_.value());
  }

  UIElement::render(context);
}

}  // namespace engine::ui
