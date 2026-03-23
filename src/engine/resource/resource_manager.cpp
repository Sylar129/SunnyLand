// Copyright Sylar129

#include "engine/resource/resource_manager.h"

#include "SDL3_ttf/SDL_ttf.h"
#include "font_manager.h"
#include "tiled_parser.h"
#include "texture_manager.h"
#include "utils/log.h"

namespace engine::resource {

ResourceManager::~ResourceManager() = default;

ResourceManager::ResourceManager(SDL_Renderer* renderer) {
  texture_manager_ = std::make_unique<TextureManager>(renderer);
  font_manager_ = std::make_unique<FontManager>();
  tiled_parser_ = std::make_unique<TiledParser>();

  ENGINE_LOG_TRACE("Init ResourceManager successfully");
}

void ResourceManager::Clear() {
  ClearTiledMaps();
  font_manager_->ClearFonts();
  texture_manager_->ClearTextures();
  ENGINE_LOG_TRACE("Clearing ResourceManager Assets");
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

std::shared_ptr<const TiledMap> ResourceManager::LoadTiledMap(
    const std::string& file_path) {
  if (const auto cached_map = GetTiledMap(file_path); cached_map != nullptr) {
    return cached_map;
  }

  std::shared_ptr<TiledMap> tiled_map = tiled_parser_->LoadMap(file_path);
  if (tiled_map == nullptr) {
    ENGINE_LOG_ERROR("Failed to load Tiled map '{}'", file_path);
    return nullptr;
  }

  tiled_maps_[file_path] = tiled_map;
  return tiled_map;
}

std::shared_ptr<const TiledMap> ResourceManager::GetTiledMap(
    const std::string& file_path) {
  const auto it = tiled_maps_.find(file_path);
  if (it == tiled_maps_.end()) {
    return nullptr;
  }
  return it->second;
}

void ResourceManager::UnloadTiledMap(const std::string& file_path) {
  tiled_maps_.erase(file_path);
}

void ResourceManager::ClearTiledMaps() { tiled_maps_.clear(); }

}  // namespace engine::resource
