// Copyright Sylar129

#pragma once

#include <optional>
#include <string>

#include "SDL3/SDL_rect.h"

#include "engine/render/sprite.h"
#include "engine/ui/ui_element.h"

namespace engine::ui {

class UIImage final : public UIElement {
public:
  UIImage(const std::string& texture_id,
          const glm::vec2& position = {0.0f, 0.0f},
          const glm::vec2& size = {0.0f, 0.0f},
          const std::optional<SDL_FRect>& source_rect = std::nullopt,
          bool is_flipped = false);

  void Render(core::Context& context) override;

  const render::Sprite& GetSprite() const { return sprite_; }
  void SetSprite(const render::Sprite& sprite) { sprite_ = sprite; }

  const std::string& GetTextureId() const { return sprite_.GetTextureId(); }
  void SetTextureId(const std::string &texture_id) {
    sprite_.SetTextureId(texture_id);
  }

  const std::optional<SDL_FRect>& GetSourceRect() const {
    return sprite_.GetSourceRect();
  }
  void SetSourceRect(const std::optional<SDL_FRect>& source_rect) {
    sprite_.SetSourceRect(source_rect);
  }

  bool IsFlipped() const { return sprite_.IsFlipped(); }
  void SetFlipped(bool flipped) { sprite_.SetFlipped(flipped); }

protected:
  render::Sprite sprite_;
};

}  // namespace engine::ui
