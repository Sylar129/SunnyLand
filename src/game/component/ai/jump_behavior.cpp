// Copyright Sylar129

#include "game/component/ai/jump_behavior.h"

#include "engine/component/animation_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/transform_component.h"
#include "game/component/ai_component.h"
#include "log.h"

namespace game::component::ai {

JumpBehavior::JumpBehavior(float min_x, float max_x, const glm::vec2& jump_vel,
                           float jump_interval)
    : patrol_min_x_(min_x),
      patrol_max_x_(max_x),
      jump_vel_(jump_vel),
      jump_interval_(jump_interval) {
  if (patrol_min_x_ >= patrol_max_x_) {
    patrol_min_x_ = patrol_max_x_;
  }
  if (jump_interval_ <= 0.0f) {
    jump_interval_ = 2.0f;
  }
  if (jump_vel_.y > 0) {
    jump_vel_.y = -jump_vel_.y;
  }
}

void JumpBehavior::Update(float delta_time, AIComponent& ai_component) {
  auto* physics_component = ai_component.GetPhysicsComponent();
  auto* transform_component = ai_component.GetTransformComponent();
  auto* sprite_component = ai_component.GetSpriteComponent();
  auto* animation_component = ai_component.GetAnimationComponent();
  if (!physics_component || !transform_component || !sprite_component ||
      !animation_component) {
    GAME_ERROR(
        "JumpBehavior missing required components. Cannot execute behavior.");
    return;
  }

  auto is_on_ground = physics_component->HasCollidedBelow();
  if (is_on_ground) {
    jump_timer_ += delta_time;
    physics_component->SetVelocityX(0.0f);

    if (jump_timer_ >= jump_interval_) {
      jump_timer_ = 0.0f;

      auto current_x = transform_component->GetPosition().x;
      if (jumping_right_ && (physics_component->HasCollidedRight() ||
                             current_x >= patrol_max_x_)) {
        jumping_right_ = false;
      } else if (!jumping_right_ && (physics_component->HasCollidedLeft() ||
                                     current_x <= patrol_min_x_)) {
        jumping_right_ = true;
      }
      auto jump_vel_x = jumping_right_ ? jump_vel_.x : -jump_vel_.x;
      physics_component->SetVelocity({jump_vel_x, jump_vel_.y});
      animation_component->PlayAnimation("jump");
      sprite_component->SetFlipped(jumping_right_);

    } else {
      animation_component->PlayAnimation("idle");
    }
  } else {
    if (physics_component->GetVelocity().y < 0) {
      animation_component->PlayAnimation("jump");
    } else {
      animation_component->PlayAnimation("fall");
    }
  }
}

}  // namespace game::component::ai
