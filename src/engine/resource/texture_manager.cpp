// Copyright Sylar129

#include "engine/resource/texture_manager.h"

#include "SDL3_image/SDL_image.h"
#include "log.h"

namespace engine::resource {
TextureManager::TextureManager(SDL_Renderer* renderer) : renderer_(renderer) {
  if (!renderer_) {
    ENGINE_CRITICAL("Failed to init TextureManager:  SDL_Renderer is empty");
  }
  ENGINE_TRACE("Init TextureManager Successfully");
}

SDL_Texture* TextureManager::LoadTexture(const std::string& file_path) {
  auto it = textures_.find(file_path);
  if (it != textures_.end()) {
    return it->second.get();
  }

  SDL_Texture* raw_texture = IMG_LoadTexture(renderer_, file_path.c_str());

  if (!raw_texture) {
    ENGINE_ERROR("Failed to load texture: '{}': {}", file_path, SDL_GetError());
    return nullptr;
  }

  textures_.emplace(
      file_path, std::unique_ptr<SDL_Texture, SDLTextureDeleter>(raw_texture));
  ENGINE_DEBUG("Successfully loaded and cached texture: {}", file_path);

  return raw_texture;
}

SDL_Texture* TextureManager::GetTexture(const std::string& file_path) {
  auto it = textures_.find(file_path);
  if (it != textures_.end()) {
    return it->second.get();
  }

  ENGINE_WARN("Texture '{}' not found. Try loading", file_path);
  return LoadTexture(file_path);
}

glm::vec2 TextureManager::GetTextureSize(const std::string& file_path) {
  SDL_Texture* texture = GetTexture(file_path);
  if (!texture) {
    ENGINE_WARN("Failed to get texture: {}", file_path);
    return glm::vec2(0);
  }

  glm::vec2 size;
  if (!SDL_GetTextureSize(texture, &size.x, &size.y)) {
    ENGINE_ERROR("Faied to get texture size: {}", file_path);
    return glm::vec2(0);
  }
  return size;
}

void TextureManager::UnloadTexture(const std::string& file_path) {
  auto it = textures_.find(file_path);
  if (it != textures_.end()) {
    ENGINE_DEBUG("Unloading texture: {}", file_path);
    textures_.erase(it);
  } else {
    ENGINE_WARN("Trying to unload non-existent texture: {}", file_path);
  }
}

void TextureManager::ClearTextures() {
  if (!textures_.empty()) {
    ENGINE_DEBUG("Clearing all {} cached texture.", textures_.size());
    textures_.clear();
  }
}

}  // namespace engine::resource