#include "ui_image.h"

#include <spdlog/spdlog.h>

#include "../core/context.h"
#include "../render/renderer.h"
#include "../render/sprite.h"

namespace engine::ui {

UIImage::UIImage(const std::string& texture_id, const glm::vec2& position,
                 const glm::vec2& size,
                 const std::optional<SDL_FRect>& source_rect, bool is_flipped)
    : UIElement(position, size), sprite_(texture_id, source_rect, is_flipped) {
  if (texture_id.empty()) {
    spdlog::warn("创建了一个空纹理ID的UIImage。");
  }
  spdlog::trace("UIImage 构造完成");
}

void UIImage::Render(engine::core::Context& context) {
  if (!visible_ || sprite_.GetTextureId().empty()) {
    return;  // 如果不可见或没有分配纹理则不渲染
  }

  // 渲染自身
  auto position = GetScreenPosition();
  if (size_.x == 0.0f &&
      size_.y == 0.0f) {  // 如果尺寸为0，则使用纹理的原始尺寸
    context.GetRenderer().DrawUISprite(sprite_, position);
  } else {
    context.GetRenderer().DrawUISprite(sprite_, position, size_);
  }

  // 渲染子元素（调用基类方法）
  UIElement::Render(context);
}

}  // namespace engine::ui