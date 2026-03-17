// Copyright Sylar129

#include "engine/resource/texture_manager.h"

#include "SDL3_image/SDL_image.h"
#include "engine/utils/assert.h"
#include "log.h"

namespace engine::resource {
TextureManager::TextureManager(SDL_Renderer* renderer) : renderer_(renderer) {
  ENGINE_LOG_ASSERT(renderer_,
                    "Failed to init TextureManager: SDL_Renderer is nullptr");
  ENGINE_LOG_TRACE("Init TextureManager successfully");
}

SDL_Texture* TextureManager::LoadTexture(const std::string& file_path) {
  auto it = textures_.find(file_path);
  if (it != textures_.end()) {
    return it->second.get();
  }

  SDL_Texture* raw_texture = IMG_LoadTexture(renderer_, file_path.c_str());

  if (!raw_texture) {
    ENGINE_LOG_ERROR("Failed to load texture: '{}': {}", file_path,
                     SDL_GetError());
    return nullptr;
  }
  if (!SDL_SetTextureScaleMode(raw_texture, SDL_SCALEMODE_NEAREST)) {
    ENGINE_LOG_WARN("Can't set ScaleMode to SDL_SCALEMODE_NEAREST");
  }

  textures_.emplace(
      file_path, std::unique_ptr<SDL_Texture, SDLTextureDeleter>(raw_texture));
  ENGINE_LOG_DEBUG("Successfully loaded and cached texture: {}", file_path);

  return raw_texture;
}

SDL_Texture* TextureManager::GetTexture(const std::string& file_path) {
  auto it = textures_.find(file_path);
  if (it != textures_.end()) {
    return it->second.get();
  }

  ENGINE_LOG_WARN("Texture '{}' not found. Try loading", file_path);
  return LoadTexture(file_path);
}

glm::vec2 TextureManager::GetTextureSize(const std::string& file_path) {
  SDL_Texture* texture = GetTexture(file_path);
  if (!texture) {
    ENGINE_LOG_WARN("Failed to get texture: {}", file_path);
    return glm::vec2(0);
  }

  glm::vec2 size;
  if (!SDL_GetTextureSize(texture, &size.x, &size.y)) {
    ENGINE_LOG_ERROR("Faied to get texture size: {}", file_path);
    return glm::vec2(0);
  }
  return size;
}

void TextureManager::UnloadTexture(const std::string& file_path) {
  auto it = textures_.find(file_path);
  if (it != textures_.end()) {
    ENGINE_LOG_DEBUG("Unloading texture: {}", file_path);
    textures_.erase(it);
  } else {
    ENGINE_LOG_WARN("Trying to unload non-existent texture: {}", file_path);
  }
}

void TextureManager::ClearTextures() {
  if (!textures_.empty()) {
    ENGINE_LOG_DEBUG("Clearing all {} cached texture.", textures_.size());
    textures_.clear();
  }
}

}  // namespace engine::resource
