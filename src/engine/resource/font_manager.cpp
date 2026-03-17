// Copyright Sylar129

#include "engine/resource/font_manager.h"

#include "utils/log.h"

namespace engine::resource {

FontManager::FontManager() {
  if (!TTF_WasInit() && !TTF_Init()) {
    ENGINE_LOG_CRITICAL("FontManager Error: TTF_Init failed: {}",
                        SDL_GetError());
  }
  ENGINE_LOG_TRACE("Init FontManager successfully");
}

FontManager::~FontManager() {
  if (!fonts_.empty()) {
    ENGINE_LOG_DEBUG("FontManager is not empty. ClearFonts first.");
    ClearFonts();
  }
  TTF_Quit();
  ENGINE_LOG_TRACE("Deinit FontManager successfully");
}

TTF_Font* FontManager::LoadFont(const std::string& file_path, int point_size) {
  if (point_size <= 0) {
    ENGINE_LOG_ERROR("Falied to load font: {}. Invalid point size: {}",
                     file_path, point_size);
    return nullptr;
  }

  FontKey key = {file_path, point_size};

  auto it = fonts_.find(key);
  if (it != fonts_.end()) {
    return it->second.get();
  }

  ENGINE_LOG_DEBUG("Loading font: {} ({}pt)", file_path, point_size);
  TTF_Font* raw_font = TTF_OpenFont(file_path.c_str(), point_size);
  if (!raw_font) {
    ENGINE_LOG_ERROR("Loading font '{}' ({}pt) falied: {}", file_path,
                     point_size, SDL_GetError());
    return nullptr;
  }

  fonts_.emplace(key, std::unique_ptr<TTF_Font, SDLFontDeleter>(raw_font));
  ENGINE_LOG_DEBUG("Successfully load and cache font: {} ({}pt)", file_path,
                   point_size);
  return raw_font;
}

TTF_Font* FontManager::GetFont(const std::string& file_path, int point_size) {
  FontKey key = {file_path, point_size};
  auto it = fonts_.find(key);
  if (it != fonts_.end()) {
    return it->second.get();
  }

  ENGINE_LOG_WARN("Font '{}' ({}pt) is not in cache. Try loading", file_path,
                  point_size);
  return LoadFont(file_path, point_size);
}

void FontManager::UnloadFont(const std::string& file_path, int point_size) {
  FontKey key = {file_path, point_size};
  auto it = fonts_.find(key);
  if (it != fonts_.end()) {
    ENGINE_LOG_DEBUG("Unload font: {} ({}pt)", file_path, point_size);
    fonts_.erase(it);
  } else {
    ENGINE_LOG_WARN("Trying to unload non-existent font: {} ({}pt)", file_path,
                    point_size);
  }
}

void FontManager::ClearFonts() {
  if (!fonts_.empty()) {
    ENGINE_LOG_DEBUG("Clearing all {} cached fonts。", fonts_.size());
    fonts_.clear();
  }
}

}  // namespace engine::resource
