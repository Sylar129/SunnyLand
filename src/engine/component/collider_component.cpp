// Copyright Sylar129

#include "engine/component/collider_component.h"

#include "engine/component/transform_component.h"
#include "engine/object/game_object.h"
#include "engine/physics/collider.h"
#include "utils/assert.h"
#include "utils/log.h"

namespace engine::component {

ColliderComponent::ColliderComponent(
    std::unique_ptr<engine::physics::Collider> collider,
    engine::utils::Alignment alignment, bool is_trigger, bool is_active)
    : collider_(std::move(collider)),
      alignment_(alignment),
      is_trigger_(is_trigger),
      is_active_(is_active) {
  ENGINE_LOG_ASSERT(collider_,
                    "Empty collider passed to ColliderComponent constructor!");
}

void ColliderComponent::Init() {
  ENGINE_LOG_ASSERT(owner_, "ColliderComponent has no owner GameObject!");

  transform_ = owner_->GetComponent<TransformComponent>();
  if (!transform_) {
    ENGINE_LOG_ERROR("ColliderComponent: No TransformComponent!");
    return;
  }

  UpdateOffset();
}

void ColliderComponent::SetAlignment(engine::utils::Alignment anchor) {
  alignment_ = anchor;
  if (transform_ && collider_) {
    UpdateOffset();
  }
}

void ColliderComponent::UpdateOffset() {
  if (!collider_) return;

  auto collider_size = collider_->GetAABBSize();

  if (collider_size.x <= 0.0f || collider_size.y <= 0.0f) {
    offset_ = {0.0f, 0.0f};
    return;
  }
  auto scale = transform_->GetScale();

  switch (alignment_) {
    case engine::utils::Alignment::kTopLeft:
      offset_ = glm::vec2{0.0f, 0.0f} * scale;
      break;
    case engine::utils::Alignment::kTopCenter:
      offset_ = glm::vec2{-collider_size.x / 2.0f, 0.0f} * scale;
      break;
    case engine::utils::Alignment::kTopRight:
      offset_ = glm::vec2{-collider_size.x, 0.0f} * scale;
      break;
    case engine::utils::Alignment::kCenterLeft:
      offset_ = glm::vec2{0.0f, -collider_size.y / 2.0f} * scale;
      break;
    case engine::utils::Alignment::kCenter:
      offset_ =
          glm::vec2{-collider_size.x / 2.0f, -collider_size.y / 2.0f} * scale;
      break;
    case engine::utils::Alignment::kCenterRight:
      offset_ = glm::vec2{-collider_size.x, -collider_size.y / 2.0f} * scale;
      break;
    case engine::utils::Alignment::kBottomLeft:
      offset_ = glm::vec2{0.0f, -collider_size.y} * scale;
      break;
    case engine::utils::Alignment::kBottomCenter:
      offset_ = glm::vec2{-collider_size.x / 2.0f, -collider_size.y} * scale;
      break;
    case engine::utils::Alignment::kBottomRight:
      offset_ = glm::vec2{-collider_size.x, -collider_size.y} * scale;
      break;
    default:
      break;
  }
}

engine::utils::Rect ColliderComponent::GetWorldAABB() const {
  if (!transform_ || !collider_) {
    return {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
  }
  const glm::vec2 top_left_pos = transform_->GetPosition() + offset_;
  const glm::vec2 base_size = collider_->GetAABBSize();
  const glm::vec2 scale = transform_->GetScale();
  glm::vec2 scaled_size = base_size * scale;
  return {top_left_pos, scaled_size};
}

}  // namespace engine::component
