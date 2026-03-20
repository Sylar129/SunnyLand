// Copyright Sylar129

#pragma once

#include "engine/component/component.h"
#include "utils/math.h"

namespace engine::component {
class TransformComponent;
}

namespace engine::component {

class ColliderComponent final : public Component {
  friend class object::GameObject;

 public:
  explicit ColliderComponent(const glm::vec2& size, bool is_trigger = false,
                             bool is_active = true);

  const glm::vec2& GetOffset() const { return offset_; }
  utils::Rect GetWorldAABB() const;
  bool IsTrigger() const { return is_trigger_; }
  bool IsActive() const { return is_active_; }

  void SetOffset(const glm::vec2& offset) { offset_ = offset; }
  void SetTrigger(bool is_trigger) { is_trigger_ = is_trigger; }
  void SetActive(bool is_active) { is_active_ = is_active; }

 private:
  void Init() override;
  void Update(float, core::Context&) override {}

  glm::vec2 size_ = {0.0f, 0.0f};
  glm::vec2 offset_ = {0.0f, 0.0f};

  bool is_trigger_ = false;
  bool is_active_ = true;
};

}  // namespace engine::component
