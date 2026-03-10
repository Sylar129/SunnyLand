// Copyright Sylar129

#include "engine/component/animation_component.h"

#include <spdlog/spdlog.h>

#include "engine/component/sprite_component.h"
#include "engine/object/game_object.h"
#include "engine/render/animation.h"

namespace engine::component {

AnimationComponent::~AnimationComponent() = default;

void AnimationComponent::Init() {
  if (!owner_) {
    spdlog::error("AnimationComponent 没有所有者 GameObject！");
    return;
  }
  sprite_component_ = owner_->GetComponent<SpriteComponent>();
  if (!sprite_component_) {
    spdlog::error(
        "GameObject '{}' 的 AnimationComponent 需要 "
        "SpriteComponent，但未找到。",
        owner_->GetName());
    return;
  }
}

void AnimationComponent::Update(float delta_time, engine::core::Context&) {
  if (!is_playing_ || !current_animation_ || !sprite_component_ ||
      current_animation_->isEmpty()) {
    spdlog::trace(
        "AnimationComponent 更新时没有正在播放的动画或精灵组件为空。");
    return;
  }

  animation_timer_ += delta_time;

  const auto& current_frame = current_animation_->getFrame(animation_timer_);

  sprite_component_->SetSourceRect(current_frame.source_rect);

  if (!current_animation_->isLooping() &&
      animation_timer_ >= current_animation_->getTotalDuration()) {
    is_playing_ = false;
    animation_timer_ = current_animation_->getTotalDuration();
    if (is_one_shot_removal_) {
      owner_->SetNeedRemove(true);
    }
  }
}

void AnimationComponent::addAnimation(
    std::unique_ptr<engine::render::Animation> animation) {
  if (!animation) return;
  std::string name = animation->getName();
  animations_[name] = std::move(animation);
  spdlog::debug("已将动画 '{}' 添加到 GameObject '{}'", name,
                owner_ ? owner_->GetName() : "未知");
}

void AnimationComponent::playAnimation(const std::string& name) {
  auto it = animations_.find(name);
  if (it == animations_.end() || !it->second) {
    spdlog::warn("未找到 GameObject '{}' 的动画 '{}'", name,
                 owner_ ? owner_->GetName() : "未知");
    return;
  }

  if (current_animation_ == it->second.get() && is_playing_) {
    return;
  }

  current_animation_ = it->second.get();
  animation_timer_ = 0.0f;
  is_playing_ = true;

  if (sprite_component_ && !current_animation_->isEmpty()) {
    const auto& first_frame = current_animation_->getFrame(0.0f);
    sprite_component_->SetSourceRect(first_frame.source_rect);
    spdlog::debug("GameObject '{}' 播放动画 '{}'",
                  owner_ ? owner_->GetName() : "未知", name);
  }
}

std::string AnimationComponent::getCurrentAnimationName() const {
  if (current_animation_) {
    return current_animation_->getName();
  }
  return "";
}

bool AnimationComponent::isAnimationFinished() const {
  if (!current_animation_ || current_animation_->isLooping()) {
    return false;
  }
  return animation_timer_ >= current_animation_->getTotalDuration();
}

}  // namespace engine::component
