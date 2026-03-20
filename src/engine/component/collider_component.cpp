// Copyright Sylar129

#include "engine/component/collider_component.h"

#include "engine/component/transform_component.h"
#include "engine/object/game_object.h"
#include "utils/assert.h"
#include "utils/log.h"

namespace engine::component {

ColliderComponent::ColliderComponent(const glm::vec2 &size, bool is_trigger,
                                     bool is_active)
    : size_(size), is_trigger_(is_trigger), is_active_(is_active) {}

void ColliderComponent::Init() {
  ENGINE_LOG_ASSERT(owner_, "ColliderComponent has no owner GameObject!");
}

utils::Rect ColliderComponent::GetWorldAABB() const {
  auto& transform_component = owner_->GetComponent<TransformComponent>();
  const glm::vec2 top_left_pos = transform_component.GetPosition() + offset_;
  const glm::vec2 scale = transform_component.GetScale();
  glm::vec2 scaled_size = size_ * scale;
  return {top_left_pos, scaled_size};
}

}  // namespace engine::component
