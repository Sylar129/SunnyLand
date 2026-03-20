// Copyright Sylar129

#include "engine/component/collider_component.h"

#include "engine/component/transform_component.h"
#include "engine/object/game_object.h"
#include "utils/assert.h"
#include "utils/log.h"

namespace engine::component {

ColliderComponent::ColliderComponent(const glm::vec2 &size,
                                     utils::Alignment alignment,
                                     bool is_trigger, bool is_active)
    : size_(size),
      alignment_(alignment),
      is_trigger_(is_trigger),
      is_active_(is_active) {}

void ColliderComponent::Init() {
  ENGINE_LOG_ASSERT(owner_, "ColliderComponent has no owner GameObject!");

  transform_ = owner_->GetComponent<TransformComponent>();
  if (!transform_) {
    ENGINE_LOG_ERROR("ColliderComponent: No TransformComponent!");
    return;
  }

  UpdateOffset();
}

void ColliderComponent::SetAlignment(utils::Alignment anchor) {
  alignment_ = anchor;
  if (transform_) {
    UpdateOffset();
  }
}

void ColliderComponent::UpdateOffset() {
  auto collider_size = size_;

  if (collider_size.x <= 0.0f || collider_size.y <= 0.0f) {
    offset_ = {0.0f, 0.0f};
    return;
  }
  auto scale = transform_->GetScale();

  switch (alignment_) {
    case utils::Alignment::kTopLeft:
      offset_ = glm::vec2{0.0f, 0.0f} * scale;
      break;
    case utils::Alignment::kTopCenter:
      offset_ = glm::vec2{-collider_size.x / 2.0f, 0.0f} * scale;
      break;
    case utils::Alignment::kTopRight:
      offset_ = glm::vec2{-collider_size.x, 0.0f} * scale;
      break;
    case utils::Alignment::kCenterLeft:
      offset_ = glm::vec2{0.0f, -collider_size.y / 2.0f} * scale;
      break;
    case utils::Alignment::kCenter:
      offset_ =
          glm::vec2{-collider_size.x / 2.0f, -collider_size.y / 2.0f} * scale;
      break;
    case utils::Alignment::kCenterRight:
      offset_ = glm::vec2{-collider_size.x, -collider_size.y / 2.0f} * scale;
      break;
    case utils::Alignment::kBottomLeft:
      offset_ = glm::vec2{0.0f, -collider_size.y} * scale;
      break;
    case utils::Alignment::kBottomCenter:
      offset_ = glm::vec2{-collider_size.x / 2.0f, -collider_size.y} * scale;
      break;
    case utils::Alignment::kBottomRight:
      offset_ = glm::vec2{-collider_size.x, -collider_size.y} * scale;
      break;
    default:
      break;
  }
}

utils::Rect ColliderComponent::GetWorldAABB() const {
  if (!transform_) {
    return {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
  }

  const glm::vec2 top_left_pos = transform_->GetPosition() + offset_;
  const glm::vec2 scale = transform_->GetScale();
  glm::vec2 scaled_size = size_ * scale;
  return {top_left_pos, scaled_size};
}

}  // namespace engine::component
