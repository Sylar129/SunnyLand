// Copyright Sylar129

#pragma once

#include <optional>  // For std::optional to represent optional source rectangle
#include <string>

#include "SDL3/SDL_rect.h"  // For SDL_FRect

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
  const std::string& GetTextureId() const {
    return texture_id_;
  }  ///< @brief Get texture ID
  const std::optional<SDL_FRect>& GetSourceRect() const {
    return source_rect_;
  }  ///< @brief Get source rectangle (std::nullopt if using entire texture)
  bool IsFlipped() const {
    return is_flipped_;
  }  ///< @brief Get whether to flip horizontally

  void SetTextureId(const std::string& texture_id) {
    texture_id_ = texture_id;
  }  ///< @brief Set texture ID
  void SetSourceRect(const std::optional<SDL_FRect>& source_rect) {
    source_rect_ = source_rect;
  }  ///< @brief Set source rectangle (std::nullopt if using entire texture)
  void SetFlipped(bool flipped) {
    is_flipped_ = flipped;
  }  ///< @brief Set whether to flip horizontally

 private:
  std::string texture_id_;  ///< @brief Identifier of texture resource
  std::optional<SDL_FRect>
      source_rect_;          ///< @brief Optional: texture part to draw
  bool is_flipped_ = false;  ///< @brief Whether to flip horizontally
};

}  // namespace engine::render
