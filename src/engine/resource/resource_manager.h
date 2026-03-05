// Copyright Sylar129

#pragma once

#include <memory>
#include <string>

#include "engine/utils/non_copyable.h"
#include "glm/glm.hpp"

struct SDL_Renderer;
struct SDL_Texture;
struct MIX_Audio;
struct TTF_Font;

namespace engine::resource {

class TextureManager;
class AudioManager;
class FontManager;

class ResourceManager final {
 public:
  explicit ResourceManager(SDL_Renderer* renderer);

  ~ResourceManager();

  void Clear();

 private:
  DISABLE_COPY_AND_MOVE(ResourceManager);

 public:
  SDL_Texture* LoadTexture(const std::string& file_path);
  SDL_Texture* GetTexture(const std::string& file_path);
  void UnloadTexture(const std::string& file_path);
  glm::vec2 GetTextureSize(const std::string& file_path);
  void ClearTextures();

  MIX_Audio* LoadSound(const std::string& file_path);
  MIX_Audio* GetSound(const std::string& file_path);
  void UnloadSound(const std::string& file_path);
  void ClearSounds();

  TTF_Font* LoadFont(const std::string& file_path, int point_size);
  TTF_Font* GetFont(const std::string& file_path, int point_size);
  void UnloadFont(const std::string& file_path, int point_size);
  void ClearFonts();

 private:
  std::unique_ptr<TextureManager> texture_manager_;
  std::unique_ptr<AudioManager> audio_manager_;
  std::unique_ptr<FontManager> font_manager_;
};

}  // namespace engine::resource
