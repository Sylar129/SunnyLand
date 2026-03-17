// Copyright Sylar129

#pragma once

#include <string>

#include "SDL3/SDL_render.h"
#include "glm/vec2.hpp"
#include "utils/math.h"
#include "utils/non_copyable.h"

struct TTF_TextEngine;

namespace engine::resource {
class ResourceManager;
}

namespace engine::render {
class Camera;

class TextRenderer final {
 public:
  TextRenderer(SDL_Renderer* sdl_renderer,
               engine::resource::ResourceManager* resource_manager);

  ~TextRenderer();

  DISABLE_COPY_AND_MOVE(TextRenderer);

  void Close();

  void DrawUIText(const std::string& text, const std::string& font_id,
                  int font_size, const glm::vec2& position,
                  const engine::utils::FColor& color = {1.0f, 1.0f, 1.0f,
                                                        1.0f});

  void DrawText(const Camera& camera, const std::string& text,
                const std::string& font_id, int font_size,
                const glm::vec2& position,
                const engine::utils::FColor& color = {1.0f, 1.0f, 1.0f, 1.0f});

  glm::vec2 GetTextSize(const std::string& text, const std::string& font_id,
                        int font_size);

 private:
  SDL_Renderer* sdl_renderer_ = nullptr;
  engine::resource::ResourceManager* resource_manager_ = nullptr;

  TTF_TextEngine* text_ENGINE_LOG_ = nullptr;
};

}  // namespace engine::render
