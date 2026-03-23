// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "glm/vec2.hpp"
#include "utils/non_copyable.h"

struct SDL_Renderer;
struct SDL_Texture;
struct MIX_Audio;
struct TTF_Font;

namespace engine::resource {

class TextureManager;
class FontManager;
class TiledParser;
struct TiledMap;

class ResourceManager final {
 public:
  explicit ResourceManager(SDL_Renderer* renderer);
  ~ResourceManager();
  DISABLE_COPY_AND_MOVE(ResourceManager);

  void Clear();

  SDL_Texture* LoadTexture(const std::string& file_path);
  SDL_Texture* GetTexture(const std::string& file_path);
  void UnloadTexture(const std::string& file_path);
  glm::vec2 GetTextureSize(const std::string& file_path);
  void ClearTextures();

  TTF_Font* LoadFont(const std::string& file_path, int point_size);
  TTF_Font* GetFont(const std::string& file_path, int point_size);
  void UnloadFont(const std::string& file_path, int point_size);
  void ClearFonts();

  std::shared_ptr<const TiledMap> LoadTiledMap(const std::string& file_path);
  std::shared_ptr<const TiledMap> GetTiledMap(const std::string& file_path);
  void UnloadTiledMap(const std::string& file_path);
  void ClearTiledMaps();

 private:
  std::unique_ptr<TextureManager> texture_manager_;
  std::unique_ptr<FontManager> font_manager_;
  std::unique_ptr<TiledParser> tiled_parser_;
  std::unordered_map<std::string, std::shared_ptr<const TiledMap>> tiled_maps_;
};

}  // namespace engine::resource
