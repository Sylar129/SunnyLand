// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/component/component.h"
#include "engine/physics/collider.h"
#include "utils/alignment.h"
#include "utils/math.h"

namespace engine::component {
class TransformComponent;
}

namespace engine::component {

class ColliderComponent final : public Component {
  friend class object::GameObject;

 public:
  explicit ColliderComponent(
      std::unique_ptr<physics::Collider> collider,
      utils::Alignment alignment = utils::Alignment::kNone,
      bool is_trigger = false, bool is_active = true);

  void UpdateOffset();

  const TransformComponent* GetTransform() const { return transform_; }
  const physics::Collider* GetCollider() const { return collider_.get(); }
  const glm::vec2& GetOffset() const { return offset_; }
  utils::Alignment GetAlignment() const { return alignment_; }
  utils::Rect GetWorldAABB() const;
  bool IsTrigger() const { return is_trigger_; }
  bool IsActive() const { return is_active_; }

  void SetAlignment(utils::Alignment anchor);
  void SetOffset(const glm::vec2& offset) { offset_ = offset; }
  void SetTrigger(bool is_trigger) { is_trigger_ = is_trigger; }
  void SetActive(bool is_active) { is_active_ = is_active; }

 private:
  void Init() override;
  void Update(float, core::Context&) override {}

  TransformComponent* transform_ = nullptr;

  std::unique_ptr<physics::Collider> collider_;
  glm::vec2 offset_ = {0.0f, 0.0f};
  utils::Alignment alignment_ = utils::Alignment::kNone;

  bool is_trigger_ = false;
  bool is_active_ = true;
};

}  // namespace engine::component
