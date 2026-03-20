// Copyright Sylar129

#include "engine/render/camera.h"

#include "engine/component/transform_component.h"
#include "utils/log.h"
#include "utils/math.h"

namespace engine::render {

Camera::Camera(const glm::vec2& viewport_size, const glm::vec2& position,
               const std::optional<utils::Rect> limit_bounds)
    : viewport_size_(viewport_size),
      position_(position),
      limit_bounds_(limit_bounds) {
  ENGINE_LOG_TRACE("Camera initialized successfully, position: {},",
                   position_.x, position_.y);
}

void Camera::SetPosition(const glm::vec2& position) {
  position_ = position;
  ClampPosition();
}

void Camera::Update(float delta_time) {}

void Camera::Move(const glm::vec2& offset) {
  position_ += offset;
  ClampPosition();
}

void Camera::SetLimitBounds(const utils::Rect& bounds) {
  limit_bounds_ = bounds;
  ClampPosition();
}

const glm::vec2& Camera::GetPosition() const { return position_; }

void Camera::ClampPosition() {
  if (!limit_bounds_.has_value() || limit_bounds_->size.x <= 0 ||
      limit_bounds_->size.y <= 0) {
    return;
  }
  glm::vec2 min_cam_pos = limit_bounds_->position;
  glm::vec2 max_cam_pos =
      limit_bounds_->position + limit_bounds_->size - viewport_size_;

  max_cam_pos.x = std::max(min_cam_pos.x, max_cam_pos.x);
  max_cam_pos.y = std::max(min_cam_pos.y, max_cam_pos.y);

  position_ = glm::clamp(position_, min_cam_pos, max_cam_pos);
}

glm::vec2 Camera::WorldToScreen(const glm::vec2& world_pos) const {
  return world_pos - position_;
}

glm::vec2 Camera::WorldToScreenWithParallax(
    const glm::vec2& world_pos, const glm::vec2& scroll_factor) const {
  return world_pos - position_ * scroll_factor;
}

glm::vec2 Camera::ScreenToWorld(const glm::vec2& screen_pos) const {
  return screen_pos + position_;
}

glm::vec2 Camera::GetViewportSize() const { return viewport_size_; }

std::optional<utils::Rect> Camera::GetLimitBounds() const {
  return limit_bounds_;
}

}  // namespace engine::render
