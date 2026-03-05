// Copyright Sylar129

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include "SDL3_ttf/SDL_ttf.h"
#include "engine/utils/non_copyable.h"

namespace engine::resource {

using FontKey = std::pair<std::string, int>;

struct FontKeyHash {
  std::size_t operator()(const FontKey& key) const {
    std::hash<std::string> string_hasher;
    std::hash<int> int_hasher;
    return string_hasher(key.first) ^ int_hasher(key.second);
  }
};

class FontManager final {
  friend class ResourceManager;

 public:
  FontManager();
  ~FontManager();

 private:
  DISABLE_COPY_AND_MOVE(FontManager);

  TTF_Font* LoadFont(const std::string& file_path, int point_size);
  TTF_Font* GetFont(const std::string& file_path, int point_size);
  void UnloadFont(const std::string& file_path, int point_size);
  void ClearFonts();

 private:
  struct SDLFontDeleter {
    void operator()(TTF_Font* font) const {
      if (font) {
        TTF_CloseFont(font);
      }
    }
  };

  std::unordered_map<FontKey, std::unique_ptr<TTF_Font, SDLFontDeleter>,
                     FontKeyHash>
      fonts_;
};

}  // namespace engine::resource
