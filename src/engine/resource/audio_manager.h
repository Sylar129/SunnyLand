// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "SDL3_mixer/SDL_mixer.h"
#include "engine/utils/non_copyable.h"

namespace engine::resource {

class AudioManager final {
  friend class ResourceManager;

 public:
  AudioManager();
  ~AudioManager();
  DISABLE_COPY_AND_MOVE(AudioManager);

 private:
  MIX_Audio* LoadSound(const std::string& file_path);
  MIX_Audio* GetSound(const std::string& file_path);
  void UnloadSound(const std::string& file_path);
  void ClearSounds();
  void ClearAudio();

  struct SDLMixAudioDeleter {
    void operator()(MIX_Audio* audio) const {
      if (audio) {
        MIX_DestroyAudio(audio);
      }
    }
  };

  MIX_Mixer* mixer_;
  std::unordered_map<std::string,
                     std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>>
      sounds_;
};

}  // namespace engine::resource
