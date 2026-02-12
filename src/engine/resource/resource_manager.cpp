// Copyright Sylar129

#include "resource_manager.h"

#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "audio_manager.h"
#include "font_manager.h"
#include "spdlog/spdlog.h"
#include "texture_manager.h"

namespace engine::resource {

ResourceManager::~ResourceManager() = default;

ResourceManager::ResourceManager(SDL_Renderer* renderer) {
  texture_manager_ = std::make_unique<TextureManager>(renderer);
  audio_manager_ = std::make_unique<AudioManager>();
  font_manager_ = std::make_unique<FontManager>();

  SPDLOG_TRACE("ResourceManager 构造成功。");
}

void ResourceManager::clear() {
  font_manager_->clearFonts();
  audio_manager_->clearSounds();
  texture_manager_->clearTextures();
  SPDLOG_TRACE("ResourceManager 中的资源通过 clear() 清空。");
}

// --- 纹理接口实现 ---
SDL_Texture* ResourceManager::loadTexture(const std::string& file_path) {
  return texture_manager_->loadTexture(file_path);
}

SDL_Texture* ResourceManager::getTexture(const std::string& file_path) {
  return texture_manager_->getTexture(file_path);
}

glm::vec2 ResourceManager::getTextureSize(const std::string& file_path) {
  return texture_manager_->getTextureSize(file_path);
}

void ResourceManager::unloadTexture(const std::string& file_path) {
  texture_manager_->unloadTexture(file_path);
}

void ResourceManager::clearTextures() { texture_manager_->clearTextures(); }

// --- 音频接口实现 ---
MIX_Audio* ResourceManager::loadSound(const std::string& file_path) {
  return audio_manager_->loadSound(file_path);
}

MIX_Audio* ResourceManager::getSound(const std::string& file_path) {
  return audio_manager_->getSound(file_path);
}

void ResourceManager::unloadSound(const std::string& file_path) {
  audio_manager_->unloadSound(file_path);
}

void ResourceManager::clearSounds() { audio_manager_->clearSounds(); }

// --- 字体接口实现 ---
TTF_Font* ResourceManager::loadFont(const std::string& file_path,
                                    int point_size) {
  return font_manager_->loadFont(file_path, point_size);
}

TTF_Font* ResourceManager::getFont(const std::string& file_path,
                                   int point_size) {
  return font_manager_->getFont(file_path, point_size);
}

void ResourceManager::unloadFont(const std::string& file_path, int point_size) {
  font_manager_->unloadFont(file_path, point_size);
}

void ResourceManager::clearFonts() { font_manager_->clearFonts(); }

}  // namespace engine::resource