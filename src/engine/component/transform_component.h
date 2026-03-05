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

  TransformComponent(const TransformComponent&) = delete;
  TransformComponent& operator=(const TransformComponent&) = delete;
  TransformComponent(TransformComponent&&) = delete;
  TransformComponent& operator=(TransformComponent&&) = delete;

  const glm::vec2& getPosition() const { return position_; }
  float getRotation() const { return rotation_; }
  const glm::vec2& getScale() const { return scale_; }
  void setPosition(const glm::vec2& position) { position_ = position; }
  void setRotation(float rotation) { rotation_ = rotation; }
  void setScale(const glm::vec2& scale);
  void translate(const glm::vec2& offset) { position_ += offset; }

 private:
  void Update(float, engine::core::Context&) override {}

  glm::vec2 position_ = {0.0f, 0.0f};
  glm::vec2 scale_ = {1.0f, 1.0f};
  // degree
  float rotation_ = 0.0f;
};

}  // namespace engine::component
