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

  void clear();

  ResourceManager(const ResourceManager&) = delete;
  ResourceManager& operator=(const ResourceManager&) = delete;
  ResourceManager(ResourceManager&&) = delete;
  ResourceManager& operator=(ResourceManager&&) = delete;

  // --- 统一资源访问接口 ---
  // -- Texture --
  SDL_Texture* loadTexture(const std::string& file_path);
  SDL_Texture* getTexture(const std::string& file_path);
  void unloadTexture(const std::string& file_path);
  glm::vec2 getTextureSize(const std::string& file_path);
  void clearTextures();

  // -- Sound Effects (Chunks) --
  MIX_Audio* loadSound(const std::string& file_path);
  MIX_Audio* getSound(const std::string& file_path);
  void unloadSound(const std::string& file_path);
  void clearSounds();

  // -- Fonts --
  TTF_Font* loadFont(const std::string& file_path, int point_size);
  TTF_Font* getFont(const std::string& file_path, int point_size);
  void unloadFont(const std::string& file_path, int point_size);
  void clearFonts();

 private:
  std::unique_ptr<TextureManager> texture_manager_;
  std::unique_ptr<AudioManager> audio_manager_;
  std::unique_ptr<FontManager> font_manager_;
};

}  // namespace engine::resource


