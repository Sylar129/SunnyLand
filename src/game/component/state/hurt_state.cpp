#include "hurt_state.h"

#include "../player_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "fall_state.h"
#include "idle_state.h"
// ...

namespace game::component::state {

void HurtState::Enter() {
  PlayAnimation("hurt");  // 播放受伤动画
  // --- 造成击退效果 ---
  auto physics_component = player_component_->GetPhysicsComponent();
  auto sprite_component = player_component_->GetSpriteComponent();
  auto knockback_velocity = glm::vec2(-100.0f, -150.0f);  // 默认左上方击退
  // 根据当前精灵的朝向状态决定是否改成右上方
  if (sprite_component->IsFlipped()) {
    knockback_velocity.x = -knockback_velocity.x;  // 变成向右
  }
  physics_component->velocity_ = knockback_velocity;
}

void HurtState::Exit() {}

std::unique_ptr<PlayerState> HurtState::HandleInput(engine::core::Context&) {
  // 硬直期不能进行任何操控
  return nullptr;
}

std::unique_ptr<PlayerState> HurtState::Update(float delta_time,
                                               engine::core::Context&) {
  stunned_timer_ += delta_time;
  // --- 两种情况离开受伤（硬直）状态：---
  // 1. 落地
  auto physics_component = player_component_->GetPhysicsComponent();
  if (physics_component->HasCollidedBelow()) {
    return std::make_unique<IdleState>(player_component_);
  }
  // 2. 硬直时间结束 (能走到这里说明没有落地，直接切换到 FallState)
  if (stunned_timer_ > player_component_->getStunnedDuration()) {
    return std::make_unique<FallState>(player_component_);
  }
  return nullptr;
}
}  // namespace game::component::state
