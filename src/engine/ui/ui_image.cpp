// Copyright Sylar129

#include "engine/ui/ui_image.h"

#include "engine/core/context.h"
#include "engine/render/renderer.h"
#include "engine/render/texture.h"
#include "utils/log.h"

namespace engine::ui {

UIImage::UIImage(const std::string& texture_id, const glm::vec2& position,
                 const glm::vec2& size,
                 const std::optional<SDL_FRect>& source_rect, bool is_flipped)
    : UIElement(position, size), texture_(texture_id, source_rect, is_flipped) {
  if (texture_id.empty()) {
    ENGINE_LOG_WARN("Empty texture_id provided to UIImage constructor.");
  }
  ENGINE_LOG_TRACE("UIImage created with texture_id '{}'", texture_id);
}

void UIImage::Render(core::Context& context) {
  if (!visible_ || texture_.GetTextureId().empty()) {
    return;
  }

  auto position = GetScreenPosition();
  if (size_.x == 0.0f && size_.y == 0.0f) {
    context.GetRenderer().DrawUITexture(texture_, position);
  } else {
    context.GetRenderer().DrawUITexture(texture_, position, size_);
  }

  UIElement::Render(context);
}

}  // namespace engine::ui
