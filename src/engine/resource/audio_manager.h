// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "SDL3_mixer/SDL_mixer.h"

namespace engine::resource {

/**
 * @brief 管理 SDL_mixer 音效 (Mix_Chunk) 和音乐 (Mix_Music)。
 *
 * 提供音频资源的加载和缓存功能。构造失败时会抛出异常。
 * 仅供 ResourceManager 内部使用。
 */
class AudioManager final {
  friend class ResourceManager;

 private:
  // Mix_Chunk 的自定义删除器
  struct SDLMixAudioDeleter {
    void operator()(MIX_Audio* audio) const {
      if (audio) {
        MIX_DestroyAudio(audio);
      }
    }
  };

  // Mix_Music 的自定义删除器
  struct SDLMixTrackDeleter {
    void operator()(MIX_Track* track) const {
      if (track) {
        MIX_DestroyTrack(track);
      }
    }
  };

  MIX_Mixer* mixer_;

      // 音效存储 (文件路径 -> Mix_Chunk)
  std::unordered_map<std::string,
                     std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>>
      sounds_;

 public:
  /**
   * @brief 构造函数。初始化 SDL_mixer 并打开音频设备。
   * @throws std::runtime_error 如果 SDL_mixer 初始化或打开音频设备失败。
   */
  AudioManager();

  ~AudioManager();  ///< @brief 需要手动添加析构函数，清理资源并关闭
                    ///< SDL_mixer。

  // 当前设计中，我们只需要一个AudioManager，所有权不变，所以不需要拷贝、移动相关构造及赋值运算符
  AudioManager(const AudioManager&) = delete;
  AudioManager& operator=(const AudioManager&) = delete;
  AudioManager(AudioManager&&) = delete;
  AudioManager& operator=(AudioManager&&) = delete;

 private:  // 仅供 ResourceManager 访问的方法
  MIX_Audio* loadSound(
      const std::string& file_path);  ///< @brief 从文件路径加载音效
  MIX_Audio* getSound(
      const std::string&
          file_path);  ///< @brief
                       ///< 尝试获取已加载音效的指针，如果未加载则尝试加载
  void unloadSound(
      const std::string& file_path);  ///< @brief 卸载指定的音效资源
  void clearSounds();                 ///< @brief 清空所有音效资源

  void clearAudio();  ///< @brief 清空所有音频资源
};

}  // namespace engine::resource


