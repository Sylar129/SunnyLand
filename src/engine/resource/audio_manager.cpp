// Copyright Sylar129

#include "engine/resource/audio_manager.h"

#include "SDL3/SDL_error.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "log.h"

namespace engine::resource {

AudioManager::AudioManager() {
  if (!MIX_Init()) {
    ENGINE_LOG_CRITICAL("AudioManager Error: Mix_Init failed: {}",
                        SDL_GetError());
    return;
  }

  mixer_ = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
  if (!mixer_) {
    MIX_Quit();
    ENGINE_LOG_CRITICAL("AudioManager Error: MIX_CreateMixerDevice failed: {}",
                        SDL_GetError());
    return;
  }
  ENGINE_LOG_TRACE("Init AudioManager successfully");
}

AudioManager::~AudioManager() {
  ClearSounds();

  MIX_DestroyMixer(mixer_);

  MIX_Quit();
  ENGINE_LOG_TRACE("Deinit AudioManager successfully");
}

MIX_Audio* AudioManager::LoadSound(const std::string& file_path) {
  ENGINE_LOG_DEBUG("Loading sound: {}", file_path);
  auto it = sounds_.find(file_path);
  if (it != sounds_.end()) {
    ENGINE_LOG_WARN("sound already existed");
    return it->second.get();
  }

  MIX_Audio* raw_chunk = MIX_LoadAudio(mixer_, file_path.c_str(), true);
  if (!raw_chunk) {
    ENGINE_LOG_ERROR("Loading sound falied: '{}': {}", file_path,
                     SDL_GetError());
    return nullptr;
  }

  sounds_.emplace(file_path,
                  std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>(raw_chunk));
  ENGINE_LOG_DEBUG("Successfully load and cache sound: {}", file_path);
  return raw_chunk;
}

MIX_Audio* AudioManager::GetSound(const std::string& file_path) {
  auto it = sounds_.find(file_path);
  if (it != sounds_.end()) {
    return it->second.get();
  }
  ENGINE_LOG_WARN("Sound '{}' not found. Try Loading.", file_path);
  return LoadSound(file_path);
}

void AudioManager::UnloadSound(const std::string& file_path) {
  auto it = sounds_.find(file_path);
  if (it != sounds_.end()) {
    ENGINE_LOG_DEBUG("Unload sound: {}", file_path);
    sounds_.erase(it);
  } else {
    ENGINE_LOG_WARN("Try to unload non-existent sound: {}", file_path);
  }
}

void AudioManager::ClearSounds() {
  if (!sounds_.empty()) {
    ENGINE_LOG_DEBUG("Clear all {} sounds.", sounds_.size());
    sounds_.clear();
  }
}

void AudioManager::ClearAudio() { ClearSounds(); }

}  // namespace engine::resource
