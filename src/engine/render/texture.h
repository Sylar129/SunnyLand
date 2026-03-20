// Copyright Sylar129

#pragma once

#include <optional>
#include <string>

#include "SDL3/SDL_rect.h"

namespace engine::render {

class Texture final {
 public:
  Texture(const std::string& texture_id,
          const std::optional<SDL_FRect>& source_rect = std::nullopt,
          bool is_flipped = false)
      : texture_id_(texture_id),
        source_rect_(source_rect),
        is_flipped_(is_flipped) {}

  Texture() = default;

  const std::string& GetTextureId() const { return texture_id_; }
  const std::optional<SDL_FRect>& GetSourceRect() const { return source_rect_; }
  bool IsFlipped() const { return is_flipped_; }

  void SetTextureId(const std::string& texture_id) { texture_id_ = texture_id; }
  void SetSourceRect(const std::optional<SDL_FRect>& source_rect) {
    source_rect_ = source_rect;
  }
  void SetFlipped(bool flipped) { is_flipped_ = flipped; }

 private:
  std::string texture_id_;
  std::optional<SDL_FRect> source_rect_;
  bool is_flipped_ = false;
};

}  // namespace engine::render
