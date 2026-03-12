// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "engine/component/component.h"

namespace engine::render {
class Animation;
}
namespace engine::component {
class SpriteComponent;
}

namespace engine::component {

class AnimationComponent : public Component {
  friend class engine::object::GameObject;

 public:
  AnimationComponent() = default;
  ~AnimationComponent() override;

  void AddAnimation(std::unique_ptr<engine::render::Animation> animation);
  void PlayAnimation(const std::string& name);
  void StopAnimation() { is_playing_ = false; }
  void ResumeAnimation() { is_playing_ = true; }

  std::string GetCurrentAnimationName() const;
  bool IsPlaying() const { return is_playing_; }
  bool IsAnimationFinished() const;
  bool IsOneShotRemoval() const { return is_one_shot_removal_; }
  void SetOneShotRemoval(bool is_one_shot_removal) {
    is_one_shot_removal_ = is_one_shot_removal;
  }

 protected:
  void Init() override;
  void Update(float, engine::core::Context&) override;

 private:
  std::unordered_map<std::string, std::unique_ptr<engine::render::Animation>>
      animations_;
  SpriteComponent* sprite_component_ = nullptr;
  engine::render::Animation* current_animation_ = nullptr;

  float animation_timer_ = 0.0f;
  bool is_playing_ = false;
  bool is_one_shot_removal_ = false;
};

}  // namespace engine::component
