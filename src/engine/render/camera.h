// Copyright Sylar129

#pragma once

#include <optional>

#include "engine/utils/math.h"

namespace engine::render {

/**
 * @brief Camera class is responsible for managing camera position and viewport
 * size, and provides coordinate transformation functionalities. It also
 * contains boundaries to limit the camera's movement range.
 */
class Camera final {
 private:
  glm::vec2 viewport_size_;  ///< @brief Viewport size (screen size)
  glm::vec2
      position_;  ///< @brief World coordinates of the camera's top-left corner
  std::optional<engine::utils::Rect>
      limit_bounds_;  ///< @brief Limit the camera's movement range, null value
                      ///< means no limit

 public:
  Camera(const glm::vec2& viewport_size,
         const glm::vec2& position = glm::vec2(0.0f, 0.0f),
         const std::optional<engine::utils::Rect> limit_bounds = std::nullopt);

  void Update(float delta_time);       ///< @brief Update camera position
  void Move(const glm::vec2& offset);  ///< @brief Move camera

  glm::vec2 WorldToScreen(const glm::vec2& world_pos)
      const;  ///< @brief Convert world coordinates to screen coordinates
  glm::vec2 WorldToScreenWithParallax(const glm::vec2& world_pos,
                                      const glm::vec2& scroll_factor)
      const;  ///< @brief Convert world coordinates to screen coordinates,
              ///< considering parallax scrolling
  glm::vec2 ScreenToWorld(const glm::vec2& screen_pos)
      const;  ///< @brief Convert screen coordinates to world coordinates

  void SetPosition(const glm::vec2& position);  ///< @brief Set camera position
  void SetLimitBounds(
      const engine::utils::Rect&
          bounds);  ///< @brief Set the movement range limit for the camera

  const glm::vec2& GetPosition() const;  ///< @brief Get camera position
  std::optional<engine::utils::Rect> GetLimitBounds()
      const;  ///< @brief Get the camera's movement range limit
  glm::vec2 GetViewportSize() const;  ///< @brief Get viewport size

  // Disable copy and move semantics
  Camera(const Camera&) = delete;
  Camera& operator=(const Camera&) = delete;
  Camera(Camera&&) = delete;
  Camera& operator=(Camera&&) = delete;

 private:
  void ClampPosition();  ///< @brief Clamp camera position within boundaries
};

}  // namespace engine::render
