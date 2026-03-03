// Copyright Sylar129

#include "engine/render/camera.h"

#include "engine/utils/math.h"
#include "log.h"

namespace engine::render {

Camera::Camera(const glm::vec2& viewport_size, const glm::vec2& position,
               const std::optional<engine::utils::Rect> limit_bounds)
    : viewport_size_(viewport_size),
      position_(position),
      limit_bounds_(limit_bounds) {
  ENGINE_TRACE("Camera initialized successfully, position: {},", position_.x,
               position_.y);
}

void Camera::SetPosition(const glm::vec2& position) {
  position_ = position;
  ClampPosition();
}

void Camera::Update(float /* delta_time */) {
  // TODO Auto-follow target
}

void Camera::Move(const glm::vec2& offset) {
  position_ += offset;
  ClampPosition();
}

void Camera::SetLimitBounds(const engine::utils::Rect& bounds) {
  limit_bounds_ = bounds;
  ClampPosition();  // Apply limit immediately after setting bounds
}

const glm::vec2& Camera::GetPosition() const { return position_; }

void Camera::ClampPosition() {
  // Boundary check needs to ensure camera view (position to position +
  // viewport_size) is within limit_bounds
  if (limit_bounds_.has_value() && limit_bounds_->size.x > 0 &&
      limit_bounds_->size.y > 0) {
    // Calculate allowed camera position range
    glm::vec2 min_cam_pos = limit_bounds_->position;
    glm::vec2 max_cam_pos =
        limit_bounds_->position + limit_bounds_->size - viewport_size_;

    // Ensure max_cam_pos is not less than min_cam_pos (viewport may be larger
    // than the world)
    max_cam_pos.x = std::max(min_cam_pos.x, max_cam_pos.x);
    max_cam_pos.y = std::max(min_cam_pos.y, max_cam_pos.y);

    position_ = glm::clamp(position_, min_cam_pos, max_cam_pos);
  }
  // If limit_bounds is invalid, do not apply limit
}

glm::vec2 Camera::WorldToScreen(const glm::vec2& world_pos) const {
  // 将世界坐标减去相机左上角位置
  return world_pos - position_;
}

glm::vec2 Camera::WorldToScreenWithParallax(
    const glm::vec2& world_pos, const glm::vec2& scroll_factor) const {
  // 相机位置应用滚动因子
  return world_pos - position_ * scroll_factor;
}

glm::vec2 Camera::ScreenToWorld(const glm::vec2& screen_pos) const {
  // 将屏幕坐标加上相机左上角位置
  return screen_pos + position_;
}

glm::vec2 Camera::GetViewportSize() const { return viewport_size_; }

std::optional<engine::utils::Rect> Camera::GetLimitBounds() const {
  return limit_bounds_;
}

}  // namespace engine::render