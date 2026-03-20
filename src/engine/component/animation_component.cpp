// Copyright Sylar129

#include "engine/component/animation_component.h"

#include "engine/component/sprite_component.h"
#include "engine/object/game_object.h"
#include "engine/render/animation.h"
#include "utils/assert.h"
#include "utils/log.h"

namespace engine::component {

AnimationComponent::AnimationComponent() = default;

AnimationComponent::~AnimationComponent() = default;

void AnimationComponent::Init() {
  ENGINE_LOG_ASSERT(owner_,
                    "AnimationComponent must have an owner GameObject!");
}

void AnimationComponent::Update(float delta_time, core::Context&) {
  if (!is_playing_ || !current_animation_ || current_animation_->IsEmpty()) {
    ENGINE_LOG_TRACE(
        "AnimationComponent on GameObject '{}' is not playing or has no "
        "animation.",
        owner_->GetName());
    return;
  }

  animation_timer_ += delta_time;

  const auto& current_frame = current_animation_->GetFrame(animation_timer_);

  owner_->GetComponent<SpriteComponent>().SetSourceRect(
      current_frame.source_rect);

  if (!current_animation_->IsLooping() &&
      animation_timer_ >= current_animation_->GetTotalDuration()) {
    is_playing_ = false;
    animation_timer_ = current_animation_->GetTotalDuration();
  }
}

void AnimationComponent::AddAnimation(
    std::unique_ptr<render::Animation> animation) {
  if (!animation) return;
  std::string name = animation->GetName();
  animations_[name] = std::move(animation);
  ENGINE_LOG_DEBUG("Add animation '{}' to GameObject '{}'", name,
                   owner_->GetName());
}

void AnimationComponent::PlayAnimation(const std::string& name) {
  auto it = animations_.find(name);
  if (it == animations_.end() || !it->second) {
    ENGINE_LOG_WARN("Animation '{}' not found in GameObject '{}'", name,
                    owner_->GetName());
    return;
  }

  if (current_animation_ == it->second.get() && is_playing_) {
    return;
  }

  current_animation_ = it->second.get();
  animation_timer_ = 0.0f;
  is_playing_ = true;

  if (!current_animation_->IsEmpty()) {
    const auto& first_frame = current_animation_->GetFrame(0.0f);
    owner_->GetComponent<SpriteComponent>().SetSourceRect(
        first_frame.source_rect);
    ENGINE_LOG_DEBUG("GameObject '{}' started playing animation '{}'",
                     owner_->GetName(), name);
  }
}

std::string AnimationComponent::GetCurrentAnimationName() const {
  if (current_animation_) {
    return current_animation_->GetName();
  }
  return "";
}

bool AnimationComponent::IsAnimationFinished() const {
  if (!current_animation_ || current_animation_->IsLooping()) {
    return false;
  }
  return animation_timer_ >= current_animation_->GetTotalDuration();
}

}  // namespace engine::component
