// Copyright Sylar129

#pragma once

#include "engine/component/component.h"
#include "glm/vec2.hpp"

namespace engine::component {

class TransformComponent final : public Component {
  friend class engine::object::GameObject;

 public:
  TransformComponent(glm::vec2 position = {0.0f, 0.0f},
                     glm::vec2 scale = {1.0f, 1.0f}, float rotation = 0.0f)
      : position_(position), scale_(scale), rotation_(rotation) {}

  glm::vec2 GetPosition() const { return position_; }
  void SetPosition(const glm::vec2& position) { position_ = position; }
  void Translate(const glm::vec2& offset) { position_ += offset; }

  float GetRotation() const { return rotation_; }
  void SetRotation(float rotation) { rotation_ = rotation; }
  glm::vec2 GetScale() const { return scale_; }
  void SetScale(const glm::vec2& scale);

 private:
  void Update(float, engine::core::Context&) override {}

  glm::vec2 position_ = {0.0f, 0.0f};
  glm::vec2 scale_ = {1.0f, 1.0f};
  // degree
  float rotation_ = 0.0f;
};

}  // namespace engine::component
