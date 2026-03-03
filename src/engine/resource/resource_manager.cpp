// Copyright Sylar129

#include "engine/resource/resource_manager.h"

#include "SDL3_mixer/SDL_mixer.h"
#include "SDL3_ttf/SDL_ttf.h"
#include "audio_manager.h"
#include "font_manager.h"
#include "log.h"
#include "texture_manager.h"

namespace engine::resource {

ResourceManager::~ResourceManager() = default;

ResourceManager::ResourceManager(SDL_Renderer* renderer) {
  texture_manager_ = std::make_unique<TextureManager>(renderer);
  audio_manager_ = std::make_unique<AudioManager>();
  font_manager_ = std::make_unique<FontManager>();

  ENGINE_TRACE("Init ResourceManager successfully");
}

void ResourceManager::Clear() {
  font_manager_->ClearFonts();
  audio_manager_->ClearSounds();
  texture_manager_->ClearTextures();
  ENGINE_TRACE("Clearing ResourceManager Assets");
}

SDL_Texture* ResourceManager::LoadTexture(const std::string& file_path) {
  return texture_manager_->LoadTexture(file_path);
}

SDL_Texture* ResourceManager::GetTexture(const std::string& file_path) {
  return texture_manager_->GetTexture(file_path);
}

glm::vec2 ResourceManager::GetTextureSize(const std::string& file_path) {
  return texture_manager_->GetTextureSize(file_path);
}

void ResourceManager::UnloadTexture(const std::string& file_path) {
  texture_manager_->UnloadTexture(file_path);
}

void ResourceManager::ClearTextures() { texture_manager_->ClearTextures(); }

MIX_Audio* ResourceManager::LoadSound(const std::string& file_path) {
  return audio_manager_->LoadSound(file_path);
}

MIX_Audio* ResourceManager::GetSound(const std::string& file_path) {
  return audio_manager_->GetSound(file_path);
}

void ResourceManager::UnloadSound(const std::string& file_path) {
  audio_manager_->UnloadSound(file_path);
}

void ResourceManager::ClearSounds() { audio_manager_->ClearSounds(); }

TTF_Font* ResourceManager::LoadFont(const std::string& file_path,
                                    int point_size) {
  return font_manager_->LoadFont(file_path, point_size);
}

TTF_Font* ResourceManager::GetFont(const std::string& file_path,
                                   int point_size) {
  return font_manager_->GetFont(file_path, point_size);
}

void ResourceManager::UnloadFont(const std::string& file_path, int point_size) {
  font_manager_->UnloadFont(file_path, point_size);
}

void ResourceManager::ClearFonts() { font_manager_->ClearFonts(); }

}  // namespace engine::resource