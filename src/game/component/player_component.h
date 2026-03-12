// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/component/component.h"
#include "game/component/state/player_state.h"

namespace engine::input {
class InputManager;
}
namespace engine::component {
class TransformComponent;
class PhysicsComponent;
class SpriteComponent;
class AnimationComponent;
class HealthComponent;
}  // namespace engine::component

namespace game::component::state {
class PlayerState;
}

namespace game::component {

class PlayerComponent final : public engine::component::Component {
  friend class engine::object::GameObject;

 public:
  PlayerComponent() = default;
  ~PlayerComponent() override = default;

  engine::component::TransformComponent* GetTransformComponent() const {
    return transform_component_;
  }
  engine::component::SpriteComponent* GetSpriteComponent() const {
    return sprite_component_;
  }
  engine::component::PhysicsComponent* GetPhysicsComponent() const {
    return physics_component_;
  }
  engine::component::AnimationComponent* GetAnimationComponent() const {
    return animation_component_;
  }
  engine::component::HealthComponent* getHealthComponent() const {
    return health_component_;
  }

  void SetIsDead(bool is_dead) { is_dead_ = is_dead; }
  bool IsDead() const { return is_dead_; }
  void SetMoveForce(float move_force) { move_force_ = move_force; }
  float GetMoveForce() const { return move_force_; }
  void SetMaxSpeed(float max_speed) { max_speed_ = max_speed; }
  float GetMaxSpeed() const { return max_speed_; }
  void setClimbSpeed(float climb_speed) {
    climb_speed_ = climb_speed;
  }  ///< @brief 设置爬梯子速度
  float getClimbSpeed() const {
    return climb_speed_;
  }  ///< @brief 获取爬梯子速度
  void SetFrictionFactor(float friction_factor) {
    friction_factor_ = friction_factor;
  }
  float SetFrictionFactor() const { return friction_factor_; }
  void SetJumpVelocity(float jump_force) { jump_velocity_ = jump_force; }
  float GetJumpVelocity() const { return jump_velocity_; }
  void SetStunnedDuration(float duration) { stunned_duration_ = duration; }
  float GetStunnedDuration() const { return stunned_duration_; }

  void SetState(std::unique_ptr<state::PlayerState> new_state);

  bool TakeDamage(int damage);

 private:
  void Init() override;
  void HandleInput(engine::core::Context& context) override;
  void Update(float delta_time, engine::core::Context& context) override;

 private:
  engine::component::TransformComponent* transform_component_ = nullptr;
  engine::component::SpriteComponent* sprite_component_ = nullptr;
  engine::component::PhysicsComponent* physics_component_ = nullptr;
  engine::component::AnimationComponent* animation_component_ = nullptr;
  engine::component::HealthComponent* health_component_ = nullptr;

  std::unique_ptr<state::PlayerState> current_state_;
  bool is_dead_ = false;

  float move_force_ = 200.0f;
  float max_speed_ = 120.0f;
  float climb_speed_ = 100.0f;  ///< @brief 爬梯子速度 (像素/秒)
  float friction_factor_ = 0.85f;
  float jump_velocity_ = 350.0f;
  float stunned_duration_ = 0.4f;
};

}  // namespace game::component
