// Copyright Sylar129

#include "engine/resource/audio_manager.h"

#include <cassert>
#include <stdexcept>

#include "SDL3_mixer/SDL_mixer.h"
#include "log.h"

namespace engine::resource {

AudioManager::AudioManager() {
  if (!MIX_Init()) {
    throw std::runtime_error("AudioManager 错误: Mix_Init 失败: " +
                             std::string(SDL_GetError()));
  }

  // SDL3打开音频设备的方法。默认值：44100
  // Hz，默认格式，2声道（立体声），2048采样块大小
  mixer_ = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
  if (!mixer_) {
    MIX_Quit();  // 如果OpenAudio失败，先清理Mix_Init，再抛出异常
    throw std::runtime_error("AudioManager 错误: Mix_OpenAudio 失败: " +
                             std::string(SDL_GetError()));
  }
  ENGINE_TRACE("AudioManager 构造成功。");
}

AudioManager::~AudioManager() {
  ClearSounds();

  MIX_DestroyMixer(mixer_);

  MIX_Quit();
  ENGINE_TRACE("AudioManager 析构成功。");
}

// --- 音效管理 ---
MIX_Audio* AudioManager::LoadSound(const std::string& file_path) {
  auto it = sounds_.find(file_path);
  if (it != sounds_.end()) {
    return it->second.get();
  }

  ENGINE_DEBUG("加载音效: {}", file_path);
  MIX_Audio* raw_chunk = MIX_LoadAudio(mixer_, file_path.c_str(), true);
  if (!raw_chunk) {
    ENGINE_ERROR("加载音效失败: '{}': {}", file_path, SDL_GetError());
    return nullptr;
  }

  sounds_.emplace(file_path,
                  std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>(raw_chunk));
  ENGINE_DEBUG("成功加载并缓存音效: {}", file_path);
  return raw_chunk;
}

MIX_Audio* AudioManager::GetSound(const std::string& file_path) {
  auto it = sounds_.find(file_path);
  if (it != sounds_.end()) {
    return it->second.get();
  }
  ENGINE_WARN("音效 '{}' 未找到缓存，尝试加载。", file_path);
  return LoadSound(file_path);
}

void AudioManager::UnloadSound(const std::string& file_path) {
  auto it = sounds_.find(file_path);
  if (it != sounds_.end()) {
    ENGINE_DEBUG("卸载音效: {}", file_path);
    sounds_.erase(it);
  } else {
    ENGINE_WARN("尝试卸载不存在的音效: {}", file_path);
  }
}

void AudioManager::ClearSounds() {
  if (!sounds_.empty()) {
    ENGINE_DEBUG("正在清除所有 {} 个缓存的音效。", sounds_.size());
    sounds_.clear();  // unique_ptr处理删除
  }
}

void AudioManager::ClearAudio() { ClearSounds(); }

}  // namespace engine::resource