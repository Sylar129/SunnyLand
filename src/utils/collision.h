// Copyright Sylar129

#pragma once

#include "utils/math.h"

namespace utils {
inline bool CheckCircleOverlap(const glm::vec2& a_center, const float a_radius,
                        const glm::vec2& b_center, const float b_radius) {
  return (glm::length(a_center - b_center) < a_radius + b_radius);
}

inline bool CheckAABBOverlap(const glm::vec2& a_pos, const glm::vec2& a_size,
                            const glm::vec2& b_pos, const glm::vec2& b_size) {
  if (a_pos.x + a_size.x <= b_pos.x || a_pos.x >= b_pos.x + b_size.x ||
      a_pos.y + a_size.y <= b_pos.y || a_pos.y >= b_pos.y + b_size.y) {
    return false;
  }
  return true;
}

inline bool CheckRectOverlap(const utils::Rect& a, const utils::Rect& b) {
  return CheckAABBOverlap(a.position, a.size, b.position, b.size);
}

inline bool CheckPointInCircle(const glm::vec2& point, const glm::vec2& center,
                              const float radius) {
  return (glm::length(point - center) < radius);
}

}  // namespace engine::physics::collision
