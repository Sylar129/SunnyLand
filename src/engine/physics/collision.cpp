// Copyright Sylar129

#include "engine/physics/collision.h"

#include "engine/component/collider_component.h"
#include "engine/component/transform_component.h"

namespace engine::physics::collision {

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
