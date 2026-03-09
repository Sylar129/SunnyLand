// Copyright Sylar129

#pragma once

#include <utility>

#include "glm/vec2.hpp"

namespace engine::physics {

enum class ColliderType {
  NONE,
  AABB,
  CIRCLE,
};

class Collider {
 public:
  virtual ~Collider() = default;
  virtual ColliderType GetType() const = 0;

  void SetAABBSize(glm::vec2 size) { aabb_size_ = std::move(size); }
  const glm::vec2& GetAABBSize() const { return aabb_size_; }

 protected:
  glm::vec2 aabb_size_ = {0.0f, 0.0f};
};

class AABBCollider final : public Collider {
 public:
  explicit AABBCollider(glm::vec2 size) : size_(std::move(size)) {
    SetAABBSize(size_);
  }
  ~AABBCollider() override = default;

  ColliderType GetType() const override { return ColliderType::AABB; }
  const glm::vec2& GetSize() const { return size_; }
  void SetSize(glm::vec2 size) {
    size_ = std::move(size);
    SetAABBSize(size_);
  }

 private:
  glm::vec2 size_ = {0.0f, 0.0f};
};

class CircleCollider final : public Collider {
 public:
  explicit CircleCollider(float radius) : radius_(radius) {
    SetAABBSize(glm::vec2(radius * 2.0f, radius * 2.0f));
  }
  ~CircleCollider() override = default;

  ColliderType GetType() const override { return ColliderType::CIRCLE; }
  float GetRadius() const { return radius_; }
  void SetRadius(float radius) {
    radius_ = radius;
    SetAABBSize(glm::vec2(radius * 2.0f, radius * 2.0f));
  }

 private:
  float radius_ = 0.0f;
};

}  // namespace engine::physics
