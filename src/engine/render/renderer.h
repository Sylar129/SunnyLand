// Copyright Sylar129

#pragma once

#include <glm/glm.hpp>
#include <optional>  // For std::optional

#include "engine/utils/math.h"
#include "engine/utils/non_copyable.h"
#include "sprite.h"

struct SDL_Renderer;
struct SDL_FRect;
struct SDL_FColor;

namespace engine::resource {
class ResourceManager;
}

namespace engine::render {
class Camera;

/**
 * @brief Encapsulates SDL3 rendering operations
 *
 * Wraps SDL_Renderer and provides methods to clear screen, draw sprites, and
 * present the final image. Initializes on construction. Depends on valid
 * SDL_Renderer and ResourceManager. Construction failure throws an exception.
 */
class Renderer final {
 private:
  SDL_Renderer* renderer_ =
      nullptr;  ///< @brief Non-owning pointer to SDL_Renderer
  engine::resource::ResourceManager* resource_manager_ =
      nullptr;  ///< @brief Non-owning pointer to ResourceManager

 public:
  /**
   * @brief Constructor
   *
   * @param sdl_renderer Pointer to valid SDL_Renderer. Cannot be null.
   * @param resource_manager Pointer to valid ResourceManager. Cannot be null.
   * @throws std::runtime_error If any pointer is nullptr.
   */
  Renderer(SDL_Renderer* sdl_renderer,
           engine::resource::ResourceManager* resource_manager);
  DISABLE_COPY_AND_MOVE(Renderer);

  /**
   * @brief Draw a sprite
   *
   * @param sprite Sprite object containing texture ID, source rect, and flip
   * state.
   * @param position Top-left position in world coordinates.
   * @param scale Scale factor.
   * @param angle Rotation angle (degrees).
   */
  void DrawSprite(const Camera& camera, const Sprite& sprite,
                  const glm::vec2& position,
                  const glm::vec2& scale = {1.0f, 1.0f}, double angle = 0.0f);

  /**
   * @brief Draw parallax scrolling background
   *
   * @param sprite Sprite object containing texture ID, source rect, and flip
   * state.
   * @param position World coordinates.
   * @param scroll_factor Scroll factor.
   * @param scale Scale factor.
   */
  void DrawParallax(const Camera& camera, const Sprite& sprite,
                    const glm::vec2& position, const glm::vec2& scroll_factor,
                    const glm::bvec2& repeat = {true, true},
                    const glm::vec2& scale = {1.0f, 1.0f});

  /**
   * @brief Directly render a Sprite object for UI in screen coordinates.
   *
   * @param sprite Sprite object containing texture ID, source rect, and flip
   * state.
   * @param position Top-left position in screen coordinates.
   * @param size Optional: size of destination rectangle. If
   * std::nullopt, use Sprite's original size.
   */
  void DrawUISprite(const Sprite& sprite, const glm::vec2& position,
                    const std::optional<glm::vec2>& size = std::nullopt);

  void drawUIFilledRect(const engine::utils::Rect& rect,
                        const engine::utils::FColor& color);

  void Present();  ///< @brief Update screen, wraps SDL_RenderPresent function
  void ClearScreen();  ///< @brief Clear screen, wraps SDL_RenderClear function

  void SetDrawColor(
      Uint8 r, Uint8 g, Uint8 b,
      Uint8 a = 255);  ///< @brief Set draw color, wraps SDL_SetRenderDrawColor
                       ///< function, using Uint8 type
  void SetDrawColorFloat(float r, float g, float b,
                         float a = 1.0f);  ///< @brief Set draw color, wraps
                                           ///< SDL_SetRenderDrawColorFloat
                                           ///< function, using float type

  SDL_Renderer* GetSDLRenderer() const {
    return renderer_;
  }  ///< @brief Get underlying SDL_Renderer pointer

 private:
  std::optional<SDL_FRect> GetSpriteSrcRect(
      const Sprite&
          sprite);  ///< @brief
                    ///< Get sprite's source rectangle for rendering. Returns
                    ///< std::nullopt on error and skips drawing
  bool IsRectInViewport(
      const Camera& camera,
      const SDL_FRect& rect);  ///< @brief Check if rectangle is within viewport
                               ///< for viewport clipping
};

}  // namespace engine::render
