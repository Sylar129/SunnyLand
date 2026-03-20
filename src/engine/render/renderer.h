// Copyright Sylar129

#pragma once

#include <optional>

#include "glm/glm.hpp"
#include "sprite.h"
#include "utils/math.h"
#include "utils/non_copyable.h"

struct SDL_Renderer;
struct SDL_FRect;
struct SDL_FColor;

namespace engine::resource {
class ResourceManager;
}

namespace engine::render {
class Camera;

class Renderer final {
 public:
  Renderer(SDL_Renderer* sdl_renderer,
           resource::ResourceManager* resource_manager);
  DISABLE_COPY_AND_MOVE(Renderer);

  void DrawSprite(const Camera& camera, const Sprite& sprite,
                  const glm::vec2& position,
                  const glm::vec2& scale = {1.0f, 1.0f}, double angle = 0.0f);

  void DrawParallax(const Camera& camera, const Sprite& sprite,
                    const glm::vec2& position, const glm::vec2& scroll_factor,
                    const glm::bvec2& repeat = {true, true},
                    const glm::vec2& scale = {1.0f, 1.0f});

  void DrawUISprite(const Sprite& sprite, const glm::vec2& position,
                    const std::optional<glm::vec2>& size = std::nullopt);

  void DrawUIFilledRect(const utils::Rect& rect, const utils::FColor& color);

  void Present();
  void ClearScreen();

  void SetDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
  void SetDrawColorFloat(float r, float g, float b, float a = 1.0f);

  SDL_Renderer* GetSDLRenderer() const { return renderer_; }

 private:
  std::optional<SDL_FRect> GetSpriteSrcRect(const Sprite& sprite);
  bool IsRectInViewport(const Camera& camera, const SDL_FRect& rect);

  SDL_Renderer* renderer_ = nullptr;
  resource::ResourceManager* resource_manager_ = nullptr;
};

}  // namespace engine::render
