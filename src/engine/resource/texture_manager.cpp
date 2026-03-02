// Copyright Sylar129

#include "engine/resource/texture_manager.h"

#include <stdexcept>

#include "SDL3_image/SDL_image.h"
#include "log.h"

namespace engine::resource {
TextureManager::TextureManager(SDL_Renderer* renderer) : renderer_(renderer) {
  if (!renderer_) {
    throw std::runtime_error("TextureManager 构造失败: 渲染器指针为空。");
  }
  ENGINE_TRACE("TextureManager 构造成功。");
}

SDL_Texture* TextureManager::LoadTexture(const std::string& file_path) {
  auto it = textures_.find(file_path);
  if (it != textures_.end()) {
    return it->second.get();
  }

  SDL_Texture* raw_texture = IMG_LoadTexture(renderer_, file_path.c_str());

  if (!raw_texture) {
    ENGINE_ERROR("加载纹理失败: '{}': {}", file_path, SDL_GetError());
    return nullptr;
  }

  textures_.emplace(
      file_path, std::unique_ptr<SDL_Texture, SDLTextureDeleter>(raw_texture));
  ENGINE_DEBUG("成功加载并缓存纹理: {}", file_path);

  return raw_texture;
}

SDL_Texture* TextureManager::GetTexture(const std::string& file_path) {
  auto it = textures_.find(file_path);
  if (it != textures_.end()) {
    return it->second.get();
  }

  ENGINE_WARN("纹理 '{}' 未找到缓存，尝试加载。", file_path);
  return LoadTexture(file_path);
}

glm::vec2 TextureManager::GetTextureSize(const std::string& file_path) {
  SDL_Texture* texture = GetTexture(file_path);
  if (!texture) {
    ENGINE_WARN("无法获取纹理: {}", file_path);
    return glm::vec2(0);
  }

  glm::vec2 size;
  if (!SDL_GetTextureSize(texture, &size.x, &size.y)) {
    ENGINE_ERROR("无法查询纹理尺寸: {}", file_path);
    return glm::vec2(0);
  }
  return size;
}

void TextureManager::UnloadTexture(const std::string& file_path) {
  auto it = textures_.find(file_path);
  if (it != textures_.end()) {
    ENGINE_DEBUG("卸载纹理: {}", file_path);
    textures_.erase(it);
  } else {
    ENGINE_WARN("尝试卸载不存在的纹理: {}", file_path);
  }
}

void TextureManager::ClearTextures() {
  if (!textures_.empty()) {
    ENGINE_DEBUG("正在清除所有 {} 个缓存的纹理。", textures_.size());
    textures_.clear();
  }
}

}  // namespace engine::resource