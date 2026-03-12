// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/component/component.h"
#include "game/component/ai/ai_behavior.h"

namespace game::component::ai {
class AIBehavior;
}
namespace engine::component {
class TransformComponent;
class PhysicsComponent;
class SpriteComponent;
class AnimationComponent;
}  // namespace engine::component

namespace game::component {

class AIComponent final : public engine::component::Component {
  friend class engine::object::GameObject;

 public:
  AIComponent() = default;
  ~AIComponent() override = default;

  void setBehavior(std::unique_ptr<ai::AIBehavior> behavior);
  bool takeDamage(int damage);
  bool isAlive() const;

  engine::component::TransformComponent* getTransformComponent() const {
    return transform_component_;
  }
  engine::component::PhysicsComponent* getPhysicsComponent() const {
    return physics_component_;
  }
  engine::component::SpriteComponent* getSpriteComponent() const {
    return sprite_component_;
  }
  engine::component::AnimationComponent* getAnimationComponent() const {
    return animation_component_;
  }

 private:
  void Init() override;
  void Update(float delta_time, engine::core::Context&) override;

 private:
  std::unique_ptr<ai::AIBehavior> current_behavior_ = nullptr;

  engine::component::TransformComponent* transform_component_ = nullptr;
  engine::component::PhysicsComponent* physics_component_ = nullptr;
  engine::component::SpriteComponent* sprite_component_ = nullptr;
  engine::component::AnimationComponent* animation_component_ = nullptr;
};

}  // namespace game::component
