// Copyright Sylar129

#include "engine/render/camera.h"

#include "engine/component/transform_component.h"
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

void Camera::Update(float delta_time) {
  if (target_ == nullptr) return;
  glm::vec2 target_pos = target_->GetPosition();
  glm::vec2 desired_position =
      target_pos - viewport_size_ / 2.0f;  // 计算目标位置 (让目标位于视口中心)

  // 计算当前位置与目标位置的距离
  auto distance_ = glm::distance(position_, desired_position);
  constexpr float SNAP_THRESHOLD =
      1.0f;  // 设置一个距离阈值  (constexpr: 编译时常量，避免每次调用都计算)

  if (distance_ < SNAP_THRESHOLD) {
    // 如果距离小于阈值，直接吸附到目标位置
    position_ = desired_position;
  } else {
    // 否则，使用线性插值平滑移动   glm::mix(a,b,t): 在向量 a 和 b
    // 之间进行插值，t 是插值因子，范围在0到1之间。 公式: (b-a)*t + a;   t = 0
    // 时结果为 a，t = 1 时结果为 b
    position_ =
        glm::mix(position_, desired_position, smooth_speed_ * delta_time);
    position_ = glm::vec2(
        glm::round(position_.x),
        glm::round(position_.y));  // 四舍五入到整数,省略的话偶尔会出现画面割裂
  }

  ClampPosition();
}

void Camera::Move(const glm::vec2& offset) {
  position_ += offset;
  ClampPosition();
}

void Camera::SetLimitBounds(const engine::utils::Rect& bounds) {
  limit_bounds_ = bounds;
  ClampPosition();  // Apply limit immediately after setting bounds
}

void Camera::setTarget(engine::component::TransformComponent* target) {
  target_ = target;
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

engine::component::TransformComponent* Camera::getTarget() const {
  return target_;
}

std::optional<engine::utils::Rect> Camera::GetLimitBounds() const {
  return limit_bounds_;
}

}  // namespace engine::render
