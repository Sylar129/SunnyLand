// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/component/component.h"
#include "engine/utils/non_copyable.h"
#include "game/component/state/player_state.h"

namespace engine::input {
class InputManager;
}
namespace engine::component {
class TransformComponent;
class PhysicsComponent;
class SpriteComponent;
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

  DISABLE_COPY_AND_MOVE(PlayerComponent);

  engine::component::TransformComponent* getTransformComponent() const {
    return transform_component_;
  }
  engine::component::SpriteComponent* getSpriteComponent() const {
    return sprite_component_;
  }
  engine::component::PhysicsComponent* getPhysicsComponent() const {
    return physics_component_;
  }

  void setIsDead(bool is_dead) { is_dead_ = is_dead; }
  bool isDead() const { return is_dead_; }
  void setMoveForce(float move_force) { move_force_ = move_force; }
  float getMoveForce() const { return move_force_; }
  void setMaxSpeed(float max_speed) { max_speed_ = max_speed; }
  float getMaxSpeed() const { return max_speed_; }
  void setFrictionFactor(float friction_factor) {
    friction_factor_ = friction_factor;
  }
  float getFrictionFactor() const { return friction_factor_; }
  void setJumpForce(float jump_force) { jump_force_ = jump_force; }
  float getJumpForce() const { return jump_force_; }

  void setState(std::unique_ptr<state::PlayerState> new_state);

 private:
  void Init() override;
  void HandleInput(engine::core::Context& context) override;
  void Update(float delta_time, engine::core::Context& context) override;

 private:
  engine::component::TransformComponent* transform_component_ = nullptr;
  engine::component::SpriteComponent* sprite_component_ = nullptr;
  engine::component::PhysicsComponent* physics_component_ = nullptr;

  std::unique_ptr<state::PlayerState> current_state_;
  bool is_dead_ = false;

  float move_force_ = 200.0f;
  float max_speed_ = 120.0f;
  float friction_factor_ = 0.85f;
  float jump_force_ = 350.0f;
};

}  // namespace game::component
