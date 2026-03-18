// Copyright Sylar129

#include "engine/resource/audio_manager.h"

#include "SDL3/SDL_error.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "utils/log.h"

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
  ClearAudio();

  MIX_DestroyMixer(mixer_);

  MIX_Quit();
  ENGINE_LOG_TRACE("Deinit AudioManager successfully");
}

MIX_Audio* AudioManager::LoadAudio(const std::string& file_path,
                                   const AudioId& id) {
  ENGINE_LOG_DEBUG("Loading audio: {}", file_path);
  auto audio_id = id.empty() ? file_path : id;
  if (audio_.contains(audio_id)) {
    ENGINE_LOG_WARN("audio already existed");
    return audio_[audio_id].get();
  }

  MIX_Audio* raw_audio = MIX_LoadAudio(mixer_, file_path.c_str(), true);
  if (!raw_audio) {
    ENGINE_LOG_ERROR("Loading audio failed: '{}': {}", file_path,
                     SDL_GetError());
    return nullptr;
  }

  audio_[audio_id] = std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>(raw_audio);
  ENGINE_LOG_DEBUG("Successfully load and cache audio: {}", audio_id);
  return raw_audio;
}

MIX_Audio* AudioManager::GetAudio(const AudioId& id) {
  if (audio_.contains(id)) {
    return audio_[id].get();
  }
  ENGINE_LOG_WARN("audio '{}' not found.", id);
  return nullptr;
}

bool AudioManager::PlayAudio(const AudioId& id) {
  bool result = MIX_PlayAudio(mixer_, GetAudio(id));
  if (!result) {
    ENGINE_LOG_ERROR("Failed to play audio '{}': {}", id, SDL_GetError());
  }
  return result;
}

void AudioManager::UnloadAudio(const std::string& id) { audio_.erase(id); }

void AudioManager::ClearAudio() {
  if (!audio_.empty()) {
    ENGINE_LOG_DEBUG("Clear all {} audio.", audio_.size());
    audio_.clear();
  }
}

}  // namespace engine::resource
