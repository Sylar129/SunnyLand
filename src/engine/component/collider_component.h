// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/component/component.h"
#include "engine/physics/collider.h"
#include "engine/utils/alignment.h"
#include "engine/utils/math.h"

namespace engine::component {
class TransformComponent;
}

namespace engine::component {

class ColliderComponent final : public Component {
  friend class engine::object::GameObject;

 public:
  explicit ColliderComponent(
      std::unique_ptr<engine::physics::Collider> collider,
      engine::utils::Alignment alignment = engine::utils::Alignment::NONE,
      bool is_trigger = false, bool is_active = true);

  void updateOffset();

  const TransformComponent* getTransform() const { return transform_; }
  const engine::physics::Collider* getCollider() const {
    return collider_.get();
  }
  const glm::vec2& getOffset() const { return offset_; }
  engine::utils::Alignment getAlignment() const { return alignment_; }
  engine::utils::Rect getWorldAABB() const;
  bool isTrigger() const { return is_trigger_; }
  bool isActive() const { return is_active_; }

  void setAlignment(engine::utils::Alignment anchor);
  void setOffset(const glm::vec2& offset) { offset_ = offset; }
  void setTrigger(bool is_trigger) { is_trigger_ = is_trigger; }
  void setActive(bool is_active) { is_active_ = is_active; }

 private:
  void Init() override;
  void Update(float, engine::core::Context&) override {}

  TransformComponent* transform_ = nullptr;

  std::unique_ptr<engine::physics::Collider> collider_;
  glm::vec2 offset_ = {0.0f, 0.0f};
  engine::utils::Alignment alignment_ = engine::utils::Alignment::NONE;

  bool is_trigger_ = false;
  bool is_active_ = true;
};

}  // namespace engine::component