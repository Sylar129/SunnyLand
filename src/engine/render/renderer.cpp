// Copyright Sylar129

#include "engine/render/renderer.h"

#include "SDL3/SDL.h"
#include "engine/render/camera.h"
#include "engine/render/sprite.h"
#include "engine/resource/resource_manager.h"
#include "engine/utils/assert.h"
#include "log.h"

namespace engine::render {

// Constructor: Performs initialization, adds ResourceManager
Renderer::Renderer(SDL_Renderer* sdl_renderer,
                   engine::resource::ResourceManager* resource_manager)
    : renderer_(sdl_renderer), resource_manager_(resource_manager) {
  ENGINE_LOG_TRACE("Constructing Renderer...");

  ENGINE_LOG_ASSERT(
      renderer_,
      "Renderer construction failed: provided SDL_Renderer pointer is null.");
  ENGINE_LOG_ASSERT(resource_manager_,
                    "Renderer construction failed: provided ResourceManager "
                    "pointer is null.");

  SetDrawColor(0, 0, 0, 255);
  ENGINE_LOG_TRACE("Renderer construction successful.");
}

void Renderer::DrawSprite(const Camera& camera, const Sprite& sprite,
                          const glm::vec2& position, const glm::vec2& scale,
                          double angle) {
  auto texture = resource_manager_->GetTexture(sprite.GetTextureId());
  if (!texture) {
    ENGINE_LOG_ERROR("Failed to get texture for ID {}.", sprite.GetTextureId());
    return;
  }

  auto src_rect = GetSpriteSrcRect(sprite);
  if (!src_rect.has_value()) {
    ENGINE_LOG_ERROR("Failed to get sprite's source rectangle, ID: {}",
                     sprite.GetTextureId());
    return;
  }

  // Apply camera transformation
  glm::vec2 position_screen = camera.WorldToScreen(position);

  // Calculate destination rectangle, note that position is the sprite's
  // top-left coordinate
  float scaled_w = src_rect.value().w * scale.x;
  float scaled_h = src_rect.value().h * scale.y;
  SDL_FRect dest_rect = {position_screen.x, position_screen.y, scaled_w,
                         scaled_h};

  if (!IsRectInViewport(camera,
                        dest_rect)) {  // Viewport clipping: if sprite is
                                       // outside viewport, do not draw
    // ENGINE_LOG_INFO("Sprite is outside viewport range, ID: {}",
    //             sprite.GetTextureId());
    return;
  }

  // Execute drawing (default rotation center is the sprite's center)
  if (!SDL_RenderTextureRotated(
          renderer_, texture, &src_rect.value(), &dest_rect, angle, nullptr,
          sprite.IsFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
    ENGINE_LOG_ERROR("Failed to render rotated texture (ID: {}): {}",
                     sprite.GetTextureId(), SDL_GetError());
  }
}

void Renderer::DrawParallax(const Camera& camera, const Sprite& sprite,
                            const glm::vec2& position,
                            const glm::vec2& scroll_factor,
                            const glm::bvec2& repeat, const glm::vec2& scale) {
  auto texture = resource_manager_->GetTexture(sprite.GetTextureId());
  if (!texture) {
    ENGINE_LOG_ERROR("Failed to get texture for ID {}.", sprite.GetTextureId());
    return;
  }

  auto src_rect = GetSpriteSrcRect(sprite);
  if (!src_rect.has_value()) {
    ENGINE_LOG_ERROR("Failed to get sprite's source rectangle, ID: {}",
                     sprite.GetTextureId());
    return;
  }

  // Apply camera transformation
  glm::vec2 position_screen =
      camera.WorldToScreenWithParallax(position, scroll_factor);

  // Calculate scaled texture size
  float scaled_tex_w = src_rect.value().w * scale.x;
  float scaled_tex_h = src_rect.value().h * scale.y;

  glm::vec2 start, stop;
  glm::vec2 viewport_size = camera.GetViewportSize();

  if (repeat.x) {
    // Use glm::mod for float modulo
    start.x = glm::mod(position_screen.x, scaled_tex_w) - scaled_tex_w;
    stop.x = viewport_size.x;
  } else {
    start.x = position_screen.x;
    stop.x =
        glm::min(position_screen.x + scaled_tex_w,
                 viewport_size.x);  // End point is one texture width after, but
                                    // does not exceed viewport width
  }
  if (repeat.y) {
    start.y = glm::mod(position_screen.y, scaled_tex_h) - scaled_tex_h;
    stop.y = viewport_size.y;
  } else {
    start.y = position_screen.y;
    stop.y =
        glm::min(position_screen.y + scaled_tex_h,
                 viewport_size.y);  // End point is one texture height after,
                                    // but does not exceed viewport height
  }

  for (float y = start.y; y < stop.y; y += scaled_tex_h) {
    for (float x = start.x; x < stop.x; x += scaled_tex_w) {
      SDL_FRect dest_rect = {x, y, scaled_tex_w, scaled_tex_h};
      if (!SDL_RenderTexture(renderer_, texture, nullptr, &dest_rect)) {
        ENGINE_LOG_ERROR("Failed to render parallax texture (ID: {}): {}",
                         sprite.GetTextureId(), SDL_GetError());
        return;
      }
    }
  }
}

void Renderer::DrawUISprite(const Sprite& sprite, const glm::vec2& position,
                            const std::optional<glm::vec2>& size) {
  auto texture = resource_manager_->GetTexture(sprite.GetTextureId());
  if (!texture) {
    ENGINE_LOG_ERROR("Failed to get texture for ID {}.", sprite.GetTextureId());
    return;
  }

  auto src_rect = GetSpriteSrcRect(sprite);
  if (!src_rect.has_value()) {
    ENGINE_LOG_ERROR("Failed to get sprite's source rectangle, ID: {}",
                     sprite.GetTextureId());
    return;
  }

  SDL_FRect dest_rect = {position.x, position.y, 0,
                         0};  // First determine the top-left coordinate of the
                              // destination rectangle
  if (size.has_value()) {     // If size is provided, use the provided size
    dest_rect.w = size.value().x;
    dest_rect.h = size.value().y;
  } else {  // If size is not provided, use the original size of the texture
    dest_rect.w = src_rect.value().w;
    dest_rect.h = src_rect.value().h;
  }

  // Execute drawing (UI rotation is not considered)
  if (!SDL_RenderTextureRotated(
          renderer_, texture, &src_rect.value(), &dest_rect, 0.0, nullptr,
          sprite.IsFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
    ENGINE_LOG_ERROR("Failed to render UI Sprite (ID: {}): {}",
                     sprite.GetTextureId(), SDL_GetError());
  }
}

void Renderer::DrawUIFilledRect(const engine::utils::Rect& rect,
                                const engine::utils::FColor& color) {
  SetDrawColorFloat(color.r, color.g, color.b, color.a);
  SDL_FRect sdl_rect = {rect.position.x, rect.position.y, rect.size.x,
                        rect.size.y};
  if (!SDL_RenderFillRect(renderer_, &sdl_rect)) {
    ENGINE_LOG_ERROR("Failed to render filled rectangle: {}", SDL_GetError());
  }
  SetDrawColor(0, 0, 0, 1.0f);
}

void Renderer::SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
  if (!SDL_SetRenderDrawColor(renderer_, r, g, b, a)) {
    ENGINE_LOG_ERROR("Failed to set render draw color: {}", SDL_GetError());
  }
}

void Renderer::SetDrawColorFloat(float r, float g, float b, float a) {
  if (!SDL_SetRenderDrawColorFloat(renderer_, r, g, b, a)) {
    ENGINE_LOG_ERROR("Failed to set render draw color: {}", SDL_GetError());
  }
}

void Renderer::ClearScreen() {
  if (!SDL_RenderClear(renderer_)) {
    ENGINE_LOG_ERROR("Failed to clear renderer: {}", SDL_GetError());
  }
}

void Renderer::Present() { SDL_RenderPresent(renderer_); }

std::optional<SDL_FRect> Renderer::GetSpriteSrcRect(const Sprite& sprite) {
  SDL_Texture* texture = resource_manager_->GetTexture(sprite.GetTextureId());
  if (!texture) {
    ENGINE_LOG_ERROR("Failed to get texture for ID {}.", sprite.GetTextureId());
    return std::nullopt;
  }

  auto src_rect = sprite.GetSourceRect();
  if (src_rect.has_value()) {  // If Sprite has a specified rect, check if size
                               // is valid
    if (src_rect.value().w <= 0 || src_rect.value().h <= 0) {
      ENGINE_LOG_ERROR("Source rectangle size is invalid, ID: {}",
                       sprite.GetTextureId());
      return std::nullopt;
    }
    return src_rect;
  } else {  // Otherwise get texture size and return the entire texture size
    SDL_FRect result = {0, 0, 0, 0};
    if (!SDL_GetTextureSize(texture, &result.w, &result.h)) {
      ENGINE_LOG_ERROR("Failed to get texture size, ID: {}",
                       sprite.GetTextureId());
      return std::nullopt;
    }
    return result;
  }
}

bool Renderer::IsRectInViewport(const Camera& camera, const SDL_FRect& rect) {
  glm::vec2 viewport_size = camera.GetViewportSize();
  return rect.x + rect.w >= 0 &&
         rect.x <= viewport_size.x &&  // Equivalent to AABB collision detection
         rect.y + rect.h >= 0 && rect.y <= viewport_size.y;
}

}  // namespace engine::render
