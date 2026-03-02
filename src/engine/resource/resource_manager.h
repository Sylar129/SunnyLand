// Copyright Sylar129

#pragma once

#include <memory>
#include <string>

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
  /**
   * @brief 构造函数，执行初始化。
   * @param renderer SDL_Renderer 的指针，传递给需要它的子管理器。不能为空。
   */
  explicit ResourceManager(SDL_Renderer* renderer);

  ~ResourceManager();

  void Clear();

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;
  ResourceManager(ResourceManager&&) = delete;
  ResourceManager& operator=(ResourceManager&&) = delete;

  // --- 统一资源访问接口 ---
  // -- Texture --
  SDL_Texture* LoadTexture(const std::string& file_path);
  SDL_Texture* GetTexture(const std::string& file_path);
  void UnloadTexture(const std::string& file_path);
  glm::vec2 GetTextureSize(const std::string& file_path);
  void ClearTextures();

  // -- Sound Effects (Chunks) --
  MIX_Audio* LoadSound(const std::string& file_path);
  MIX_Audio* GetSound(const std::string& file_path);
  void UnloadSound(const std::string& file_path);
  void ClearSounds();

  // -- Fonts --
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
