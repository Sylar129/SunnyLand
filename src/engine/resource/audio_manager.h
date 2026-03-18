// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "SDL3_mixer/SDL_mixer.h"
#include "utils/non_copyable.h"

namespace engine::resource {

class AudioManager final {
 public:
  using AudioId = std::string;
  AudioManager();
  ~AudioManager();
  DISABLE_COPY_AND_MOVE(AudioManager);

  MIX_Audio* LoadAudio(const std::string& file_path, const AudioId& id = "");
  MIX_Audio* GetAudio(const AudioId& id);

  bool PlayAudio(const AudioId& id);

  void UnloadAudio(const AudioId& id);
  void ClearAudio();

 private:
  struct SDLMixAudioDeleter {
    void operator()(MIX_Audio* audio) const {
      if (audio) {
        MIX_DestroyAudio(audio);
      }
    }
  };

  MIX_Mixer* mixer_;
  std::unordered_map<AudioId, std::unique_ptr<MIX_Audio, SDLMixAudioDeleter>>
      audio_;
};

}  // namespace engine::resource
