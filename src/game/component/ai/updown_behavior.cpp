// Copyright Sylar129

#include "game/component/ai/updown_behavior.h"

#include "engine/component/animation_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/transform_component.h"
#include "game/component/ai_component.h"
#include "log.h"

namespace game::component::ai {

UpDownBehavior::UpDownBehavior(float min_y, float max_y, float speed)
    : patrol_min_y_(min_y), patrol_max_y_(max_y), move_speed_(speed) {
  if (patrol_min_y_ >= patrol_max_y_) {
    patrol_min_y_ = patrol_max_y_;
  }
}

void UpDownBehavior::Enter(AIComponent& ai_component) {
  if (auto* animation_component = ai_component.GetAnimationComponent();
      animation_component) {
    animation_component->PlayAnimation("fly");
  }

  if (auto* physics_component = ai_component.GetPhysicsComponent();
      physics_component) {
    physics_component->SetUseGravity(false);
  }
}

void UpDownBehavior::Update(float /*delta_time*/, AIComponent& ai_component) {
  auto* physics_component = ai_component.GetPhysicsComponent();
  auto* transform_component = ai_component.GetTransformComponent();
  if (!physics_component || !transform_component) {
    GAME_ERROR(
        "UpdownBehavior missing required components. Cannot execute behavior.");
    return;
  }

  auto current_y = transform_component->GetPosition().y;

  if (physics_component->HasCollidedAbove() || current_y <= patrol_min_y_) {
    physics_component->SetVelocityY(move_speed_);
    moving_down_ = true;
  } else if (physics_component->HasCollidedBelow() ||
             current_y >= patrol_max_y_) {
    physics_component->SetVelocityY(-move_speed_);
    moving_down_ = false;
  }
}

}  // namespace game::component::ai
