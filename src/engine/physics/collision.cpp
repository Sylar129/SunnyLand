// Copyright Sylar129

#include "engine/physics/collision.h"

#include "engine/component/collider_component.h"
#include "engine/component/transform_component.h"

namespace engine::physics::collision {

bool CheckCollision(const component::ColliderComponent& a,
                    const component::ColliderComponent& b) {
  auto a_collider = a.GetCollider();
  auto b_collider = b.GetCollider();
  auto a_transform = a.GetTransform();
  auto b_transform = b.GetTransform();

  // Ensure required components are present before dereferencing.
  if (!a_collider || !b_collider || !a_transform || !b_transform) {
    return false;
  }

  auto a_size = a_collider->GetAABBSize() * a_transform->GetScale();
  auto b_size = b_collider->GetAABBSize() * b_transform->GetScale();
  auto a_pos = a_transform->GetPosition() + a.GetOffset();
  auto b_pos = b_transform->GetPosition() + b.GetOffset();
  if (!CheckAABBOverlap(a_pos, a_size, b_pos, b_size)) {
    return false;
  }

  if (a_collider->GetType() == ColliderType::kAabb &&
      b_collider->GetType() == ColliderType::kAabb) {
    return true;
  } else if (a_collider->GetType() == ColliderType::kCircle &&
             b_collider->GetType() == ColliderType::kCircle) {
    auto a_center = a_pos + 0.5f * a_size;
    auto b_center = b_pos + 0.5f * b_size;
    auto a_radius = 0.5f * a_size.x;
    auto b_radius = 0.5f * b_size.x;
    return CheckCircleOverlap(a_center, a_radius, b_center, b_radius);
  } else if (a_collider->GetType() == ColliderType::kAabb &&
             b_collider->GetType() == ColliderType::kCircle) {
    auto b_center = b_pos + 0.5f * b_size;
    auto b_radius = 0.5f * b_size.x;
    auto nearest_point = glm::clamp(b_center, a_pos, a_pos + a_size);
    return CheckPointInCircle(nearest_point, b_center, b_radius);
  } else if (a_collider->GetType() == ColliderType::kCircle &&
             b_collider->GetType() == ColliderType::kAabb) {
    auto a_center = a_pos + 0.5f * a_size;
    auto a_radius = 0.5f * a_size.x;
    auto nearest_point = glm::clamp(a_center, b_pos, b_pos + b_size);
    return CheckPointInCircle(nearest_point, a_center, a_radius);
  }
  return false;
}

bool CheckCircleOverlap(const glm::vec2& a_center, const float a_radius,
                        const glm::vec2& b_center, const float b_radius) {
  return (glm::length(a_center - b_center) < a_radius + b_radius);
}

bool CheckAABBOverlap(const glm::vec2& a_pos, const glm::vec2& a_size,
                      const glm::vec2& b_pos, const glm::vec2& b_size) {
  if (a_pos.x + a_size.x <= b_pos.x || a_pos.x >= b_pos.x + b_size.x ||
      a_pos.y + a_size.y <= b_pos.y || a_pos.y >= b_pos.y + b_size.y) {
    return false;
  }
  return true;
}

bool CheckRectOverlap(const utils::Rect& a, const utils::Rect& b) {
  return CheckAABBOverlap(a.position, a.size, b.position, b.size);
}

bool CheckPointInCircle(const glm::vec2& point, const glm::vec2& center,
                        const float radius) {
  return (glm::length(point - center) < radius);
}

}  // namespace engine::physics::collision
