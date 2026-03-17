// Copyright Sylar129

#pragma once

#include "engine/render/text_renderer.h"
#include "engine/ui/ui_element.h"
#include "utils/math.h"

namespace engine::ui {

class UILabel final : public UIElement {
 public:
  UILabel(render::TextRenderer& text_renderer, const std::string& text,
          const std::string& font_id, int font_size = 16,
          const utils::FColor& text_color = {1.0f, 1.0f, 1.0f, 1.0f},
          const glm::vec2& position = {0.0f, 0.0f});

  void Render(core::Context& context) override;

  const std::string& GetText() const { return text_; }
  const std::string& GetFontId() const { return font_id_; }
  int GetFontSize() const { return font_size_; }
  const utils::FColor& GetTextFColor() const { return text_fcolor_; }

  void SetText(const std::string& text);
  void SetFontId(const std::string& font_id);
  void SetFontSize(int font_size);
  void SetTextFColor(const utils::FColor& text_fcolor);

 private:
  render::TextRenderer& text_renderer_;

  std::string text_;
  std::string font_id_;
  int font_size_;
  utils::FColor text_fcolor_ = {1.0f, 1.0f, 1.0f, 1.0f};
};

}  // namespace engine::ui
