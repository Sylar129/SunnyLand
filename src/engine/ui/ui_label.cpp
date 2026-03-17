// Copyright Sylar129

#include "engine/ui/ui_label.h"

#include "engine/core/context.h"
#include "engine/render/text_renderer.h"
#include "utils/log.h"

namespace engine::ui {

UILabel::UILabel(engine::render::TextRenderer& text_renderer,
                 const std::string& text, const std::string& font_id,
                 int font_size, const engine::utils::FColor& text_color,
                 const glm::vec2& position)
    : UIElement(position),
      text_renderer_(text_renderer),
      text_(text),
      font_id_(font_id),
      font_size_(font_size),
      text_fcolor_(text_color) {
  size_ = text_renderer_.GetTextSize(text_, font_id_, font_size_);
  ENGINE_LOG_TRACE("UILabel constructed.");
}

void UILabel::Render(engine::core::Context& context) {
  if (!visible_ || text_.empty()) return;

  text_renderer_.DrawUIText(text_, font_id_, font_size_, GetScreenPosition(),
                            text_fcolor_);

  UIElement::Render(context);
}

void UILabel::SetText(const std::string& text) {
  text_ = text;
  size_ = text_renderer_.GetTextSize(text_, font_id_, font_size_);
}

void UILabel::SetFontId(const std::string& font_id) {
  font_id_ = font_id;
  size_ = text_renderer_.GetTextSize(text_, font_id_, font_size_);
}

void UILabel::SetFontSize(int font_size) {
  font_size_ = font_size;
  size_ = text_renderer_.GetTextSize(text_, font_id_, font_size_);
}

void UILabel::SetTextFColor(const engine::utils::FColor& text_fcolor) {
  text_fcolor_ = text_fcolor;
}

}  // namespace engine::ui
