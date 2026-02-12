// Copyright Sylar129

#include "texture_manager.h"

#include <stdexcept>

#include "SDL3_image/SDL_image.h"
#include "spdlog/spdlog.h"

namespace engine::resource {
TextureManager::TextureManager(SDL_Renderer* renderer) : renderer_(renderer) {
  if (!renderer_) {
    throw std::runtime_error("TextureManager 构造失败: 渲染器指针为空。");
  }
  SPDLOG_TRACE("TextureManager 构造成功。");
}

SDL_Texture* TextureManager::loadTexture(const std::string& file_path) {
  auto it = textures_.find(file_path);
  if (it != textures_.end()) {
    return it->second.get();
  }

  SDL_Texture* raw_texture = IMG_LoadTexture(renderer_, file_path.c_str());

  if (!raw_texture) {
    SPDLOG_ERROR("加载纹理失败: '{}': {}", file_path, SDL_GetError());
    return nullptr;
  }

  textures_.emplace(
      file_path, std::unique_ptr<SDL_Texture, SDLTextureDeleter>(raw_texture));
  SPDLOG_DEBUG("成功加载并缓存纹理: {}", file_path);

  return raw_texture;
}

SDL_Texture* TextureManager::getTexture(const std::string& file_path) {
  auto it = textures_.find(file_path);
  if (it != textures_.end()) {
    return it->second.get();
  }

  SPDLOG_WARN("纹理 '{}' 未找到缓存，尝试加载。", file_path);
  return loadTexture(file_path);
}

glm::vec2 TextureManager::getTextureSize(const std::string& file_path) {
  SDL_Texture* texture = getTexture(file_path);
  if (!texture) {
    SPDLOG_WARN("无法获取纹理: {}", file_path);
    return glm::vec2(0);
  }

  glm::vec2 size;
  if (!SDL_GetTextureSize(texture, &size.x, &size.y)) {
    SPDLOG_ERROR("无法查询纹理尺寸: {}", file_path);
    return glm::vec2(0);
  }
  return size;
}

void TextureManager::unloadTexture(const std::string& file_path) {
  auto it = textures_.find(file_path);
  if (it != textures_.end()) {
    SPDLOG_DEBUG("卸载纹理: {}", file_path);
    textures_.erase(it);
  } else {
    SPDLOG_WARN("尝试卸载不存在的纹理: {}", file_path);
  }
}

void TextureManager::clearTextures() {
  if (!textures_.empty()) {
    SPDLOG_DEBUG("正在清除所有 {} 个缓存的纹理。", textures_.size());
    textures_.clear();
  }
}

}  // namespace engine::resource