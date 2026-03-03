// Copyright Sylar129

#pragma once

#include <optional>  // 用于 std::optional 表示可选的源矩形
#include <string>

#include "SDL3/SDL_rect.h"  // 用于 SDL_FRect

namespace engine::render {

/**
 * @brief Represents data of a visual sprite to be drawn.
 *
 * Contains texture identifier, texture part to draw (source rectangle), and
 * flip state. Position, scale and rotation are handled externally (e.g., by
 * SpriteComponent). Rendering is performed by Renderer class. (Pass Sprite as
 * parameter)
 */
class Sprite final {
 public:
  /**
   * @brief Construct a sprite
   *
   * @param texture_id Identifier of texture resource. Should not be empty.
   * @param source_rect Optional source rectangle (SDL_FRect), defining which
   * part of texture to use. If std::nullopt, use the entire texture.
   * @param is_flipped Whether to flip horizontally
   */
  Sprite(const std::string& texture_id,
         const std::optional<SDL_FRect>& source_rect = std::nullopt,
         bool is_flipped = false)
      : texture_id_(texture_id),
        source_rect_(source_rect),
        is_flipped_(is_flipped) {}

  // --- getters and setters ---
  const std::string& getTextureId() const {
    return texture_id_;
  }  ///< @brief Get texture ID
  const std::optional<SDL_FRect>& getSourceRect() const {
    return source_rect_;
  }  ///< @brief Get source rectangle (std::nullopt if using entire texture)
  bool isFlipped() const {
    return is_flipped_;
  }  ///< @brief Get whether to flip horizontally

  void setTextureId(const std::string& texture_id) {
    texture_id_ = texture_id;
  }  ///< @brief Set texture ID
  void setSourceRect(const std::optional<SDL_FRect>& source_rect) {
    source_rect_ = source_rect;
  }  ///< @brief Set source rectangle (std::nullopt if using entire texture)
  void setFlipped(bool flipped) {
    is_flipped_ = flipped;
  }  ///< @brief Set whether to flip horizontally

 private:
  std::string texture_id_;  ///< @brief Identifier of texture resource
  std::optional<SDL_FRect>
      source_rect_;          ///< @brief Optional: texture part to draw
  bool is_flipped_ = false;  ///< @brief Whether to flip horizontally
};

}  // namespace engine::render