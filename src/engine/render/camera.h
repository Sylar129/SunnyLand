// Copyright Sylar129

#pragma once

#include <optional>

#include "utils/math.h"
#include "utils/non_copyable.h"

namespace engine::render {

class Camera final {
 public:
  Camera(const glm::vec2& viewport_size,
         const glm::vec2& position = glm::vec2(0.0f, 0.0f),
         const std::optional<utils::Rect> limit_bounds = std::nullopt);
  DISABLE_COPY_AND_MOVE(Camera);

  void Update(float delta_time);
  void Move(const glm::vec2& offset);

  glm::vec2 WorldToScreen(const glm::vec2& world_pos) const;
  glm::vec2 WorldToScreenWithParallax(const glm::vec2& world_pos,
                                      const glm::vec2& scroll_factor) const;
  glm::vec2 ScreenToWorld(const glm::vec2& screen_pos) const;

  void SetPosition(const glm::vec2& position);
  void SetLimitBounds(const utils::Rect& bounds);

  const glm::vec2& GetPosition() const;
  std::optional<utils::Rect> GetLimitBounds() const;
  glm::vec2 GetViewportSize() const;

 private:
  void ClampPosition();

  glm::vec2 viewport_size_;
  glm::vec2 position_;
  std::optional<utils::Rect> limit_bounds_;

  float smooth_speed_ = 5.0f;
};

}  // namespace engine::render
