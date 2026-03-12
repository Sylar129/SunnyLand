// Copyright Sylar129

#include "game/component/ai/patrol_behavior.h"

#include "engine/component/animation_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/transform_component.h"
#include "game/component/ai_component.h"
#include "log.h"

namespace game::component::ai {

PatrolBehavior::PatrolBehavior(float min_x, float max_x, float speed)
    : patrol_min_x_(min_x), patrol_max_x_(max_x), move_speed_(speed) {
  if (patrol_min_x_ >= patrol_max_x_) {
    patrol_min_x_ = patrol_max_x_;
  }
}

void PatrolBehavior::Enter(AIComponent& ai_component) {
  if (auto* animation_component = ai_component.GetAnimationComponent();
      animation_component) {
    animation_component->PlayAnimation("walk");
  }
}

void PatrolBehavior::Update(float /*delta_time*/, AIComponent& ai_component) {
  auto* physics_component = ai_component.GetPhysicsComponent();
  auto* transform_component = ai_component.GetTransformComponent();
  auto* sprite_component = ai_component.GetSpriteComponent();
  if (!physics_component || !transform_component || !sprite_component) {
    GAME_ERROR(
        "PatrolBehavior missing required components. Cannot execute behavior.");
    return;
  }

  auto current_x = transform_component->GetPosition().x;

  if (physics_component->HasCollidedRight() || current_x >= patrol_max_x_) {
    physics_component->velocity_.x = -move_speed_;
    moving_right_ = false;
  } else if (physics_component->HasCollidedLeft() ||
             current_x <= patrol_min_x_) {
    physics_component->velocity_.x = move_speed_;
    moving_right_ = true;
  }

  sprite_component->SetFlipped(moving_right_);
}

}  // namespace game::component::ai
