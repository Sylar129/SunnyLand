// Copyright Sylar129

#include "engine/component/collider_component.h"

#include "engine/component/transform_component.h"
#include "engine/object/game_object.h"
#include "engine/physics/collider.h"
#include "log.h"

namespace engine::component {

ColliderComponent::ColliderComponent(
    std::unique_ptr<engine::physics::Collider> collider,
    engine::utils::Alignment alignment, bool is_trigger, bool is_active)
    : collider_(std::move(collider)),
      alignment_(alignment),
      is_trigger_(is_trigger),
      is_active_(is_active) {
  if (!collider_) {
    ENGINE_ERROR("创建 ColliderComponent 时传入了空的碰撞器！");
  }
}

void ColliderComponent::Init() {
  if (!owner_) {
    ENGINE_ERROR("ColliderComponent 没有所有者 GameObject！");
    return;
  }
  transform_ = owner_->GetComponent<TransformComponent>();
  if (!transform_) {
    ENGINE_ERROR(
        "ColliderComponent 需要一个在同一个 GameObject 上的 "
        "TransformComponent！");
    return;
  }

  updateOffset();
}

void ColliderComponent::setAlignment(engine::utils::Alignment anchor) {
  alignment_ = anchor;
  if (transform_ && collider_) {
    updateOffset();
  }
}

void ColliderComponent::updateOffset() {
  if (!collider_) return;

  auto collider_size = collider_->getAABBSize();

  if (collider_size.x <= 0.0f || collider_size.y <= 0.0f) {
    offset_ = {0.0f, 0.0f};
    return;
  }
  auto scale = transform_->GetScale();

  switch (alignment_) {
    case engine::utils::Alignment::TOP_LEFT:
      offset_ = glm::vec2{0.0f, 0.0f} * scale;
      break;
    case engine::utils::Alignment::TOP_CENTER:
      offset_ = glm::vec2{-collider_size.x / 2.0f, 0.0f} * scale;
      break;
    case engine::utils::Alignment::TOP_RIGHT:
      offset_ = glm::vec2{-collider_size.x, 0.0f} * scale;
      break;
    case engine::utils::Alignment::CENTER_LEFT:
      offset_ = glm::vec2{0.0f, -collider_size.y / 2.0f} * scale;
      break;
    case engine::utils::Alignment::CENTER:
      offset_ =
          glm::vec2{-collider_size.x / 2.0f, -collider_size.y / 2.0f} * scale;
      break;
    case engine::utils::Alignment::CENTER_RIGHT:
      offset_ = glm::vec2{-collider_size.x, -collider_size.y / 2.0f} * scale;
      break;
    case engine::utils::Alignment::BOTTOM_LEFT:
      offset_ = glm::vec2{0.0f, -collider_size.y} * scale;
      break;
    case engine::utils::Alignment::BOTTOM_CENTER:
      offset_ = glm::vec2{-collider_size.x / 2.0f, -collider_size.y} * scale;
      break;
    case engine::utils::Alignment::BOTTOM_RIGHT:
      offset_ = glm::vec2{-collider_size.x, -collider_size.y} * scale;
      break;
    default:
      break;
  }
}

engine::utils::Rect ColliderComponent::getWorldAABB() const {
  if (!transform_ || !collider_) {
    return {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
  }
  const glm::vec2 top_left_pos = transform_->GetPosition() + offset_;
  const glm::vec2 base_size = collider_->getAABBSize();
  const glm::vec2 scale = transform_->GetScale();
  glm::vec2 scaled_size = base_size * scale;
  return {top_left_pos, scaled_size};
}

}  // namespace engine::component