// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "SDL3/SDL_render.h"
#include "glm/glm.hpp"

namespace engine::resource {

class TextureManager final {
  friend class ResourceManager;

 public:
  explicit TextureManager(SDL_Renderer* renderer);

  TextureManager(const TextureManager&) = delete;
  TextureManager& operator=(const TextureManager&) = delete;
  TextureManager(TextureManager&&) = delete;
  TextureManager& operator=(TextureManager&&) = delete;

 private:
  SDL_Texture* LoadTexture(const std::string& file_path);
  SDL_Texture* GetTexture(const std::string& file_path);
  glm::vec2 GetTextureSize(const std::string& file_path);
  void UnloadTexture(const std::string& file_path);
  void ClearTextures();

  struct SDLTextureDeleter {
    void operator()(SDL_Texture* texture) const {
      if (texture) {
        SDL_DestroyTexture(texture);
      }
    }
  };

  std::unordered_map<std::string,
                     std::unique_ptr<SDL_Texture, SDLTextureDeleter>>
      textures_;

  SDL_Renderer* renderer_ = nullptr;
};

}  // namespace engine::resource
