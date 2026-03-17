// Copyright Sylar129

#include "engine/render/text_renderer.h"

#include "SDL3_ttf/SDL_ttf.h"
#include "engine/render/camera.h"
#include "engine/resource/resource_manager.h"
#include "utils/assert.h"
#include "utils/log.h"

namespace engine::render {

TextRenderer::TextRenderer(SDL_Renderer* sdl_renderer,
                           engine::resource::ResourceManager* resource_manager)
    : sdl_renderer_(sdl_renderer), resource_manager_(resource_manager) {
  ENGINE_LOG_ASSERT(
      sdl_renderer_ && resource_manager_,
      "TextRenderer need valid SDL_Renderer and ResourceManager!");
  if (!TTF_WasInit() && TTF_Init() == false) {
    ENGINE_LOG_ASSERT(false, "Failed to initialize SDL_ttf");
  }

  text_ENGINE_LOG_ = TTF_CreateRendererTextEngine(sdl_renderer_);
  ENGINE_LOG_ASSERT(text_ENGINE_LOG_, "Failed to create TTF_TextEngine");
}

TextRenderer::~TextRenderer() {
  if (text_ENGINE_LOG_) {
    Close();
  }
}

void TextRenderer::Close() {
  if (text_ENGINE_LOG_) {
    TTF_DestroyRendererTextEngine(text_ENGINE_LOG_);
    text_ENGINE_LOG_ = nullptr;
    ENGINE_LOG_TRACE("TTF_TextEngine Destroyed");
  }
  TTF_Quit();
}

void TextRenderer::DrawUIText(const std::string& text,
                              const std::string& font_id, int font_size,
                              const glm::vec2& position,
                              const utils::FColor& color) {
  TTF_Font* font = resource_manager_->GetFont(font_id, font_size);
  if (!font) {
    ENGINE_LOG_WARN("drawUIText Failed to get font: {} size {}", font_id,
                    font_size);
    return;
  }

  TTF_Text* temp_text_object =
      TTF_CreateText(text_ENGINE_LOG_, font, text.c_str(), 0);
  if (!temp_text_object) {
    ENGINE_LOG_ERROR("drawUIText Failed to create TTF_Text: {}",
                     SDL_GetError());
    return;
  }

  // draw shadow
  TTF_SetTextColorFloat(temp_text_object, 0.0f, 0.0f, 0.0f, 1.0f);
  if (!TTF_DrawRendererText(temp_text_object, position.x + 2, position.y + 2)) {
    ENGINE_LOG_ERROR("drawUIText Failed to draw shadow TTF_Text: {}",
                     SDL_GetError());
  }

  TTF_SetTextColorFloat(temp_text_object, color.r, color.g, color.b, color.a);
  if (!TTF_DrawRendererText(temp_text_object, position.x, position.y)) {
    ENGINE_LOG_ERROR("drawUIText Failed to draw TTF_Text: {}", SDL_GetError());
  }

  TTF_DestroyText(temp_text_object);
}

void TextRenderer::DrawText(const Camera& camera, const std::string& text,
                            const std::string& font_id, int font_size,
                            const glm::vec2& position,
                            const utils::FColor& color) {
  glm::vec2 position_screen = camera.WorldToScreen(position);

  DrawUIText(text, font_id, font_size, position_screen, color);
}

glm::vec2 TextRenderer::GetTextSize(const std::string& text,
                                    const std::string& font_id, int font_size) {
  TTF_Font* font = resource_manager_->GetFont(font_id, font_size);
  if (!font) {
    ENGINE_LOG_WARN("getTextSize Failed to get font: {} size {}", font_id,
                    font_size);
    return glm::vec2(0.0f, 0.0f);
  }

  TTF_Text* temp_text_object =
      TTF_CreateText(text_ENGINE_LOG_, font, text.c_str(), 0);
  if (!temp_text_object) {
    ENGINE_LOG_ERROR("getTextSize failed to create TTF_Text: {}",
                     SDL_GetError());
    return glm::vec2(0.0f, 0.0f);
  }

  int width, height;
  TTF_GetTextSize(temp_text_object, &width, &height);

  TTF_DestroyText(temp_text_object);

  return glm::vec2(static_cast<float>(width), static_cast<float>(height));
}

}  // namespace engine::render