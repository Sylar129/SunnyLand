// Copyright Sylar129

#pragma once

#include <string>
#include <vector>

#include "SDL3/SDL_rect.h"
#include "engine/utils/non_copyable.h"

namespace engine::render {

struct AnimationFrame {
  SDL_FRect source_rect;
  float duration;
};

class Animation final {
 public:
  Animation(const std::string& name = "default", bool loop = true);
  ~Animation() = default;

  DISABLE_COPY_AND_MOVE(Animation);

  void AddFrame(const SDL_FRect& source_rect, float duration);

  const AnimationFrame& GetFrame(float time) const;

  const std::string& GetName() const { return name_; }
  const std::vector<AnimationFrame>& GetFrames() const { return frames_; }
  size_t GetFrameCount() const { return frames_.size(); }
  float GetTotalDuration() const { return total_duration_; }
  bool IsLooping() const { return loop_; }
  bool IsEmpty() const { return frames_.empty(); }

  void SetName(const std::string& name) { name_ = name; }
  void SetLooping(bool loop) { loop_ = loop; }

 private:
  std::string name_;
  std::vector<AnimationFrame> frames_;
  float total_duration_ = 0.0f;
  bool loop_ = true;
};

}  // namespace engine::render
