#include <spdlog/spdlog.h>

#include <glm/common.hpp>

#include "../../../engine/component/animation_component.h"
#include "../../../engine/component/physics_component.h"
#include "../../../engine/component/sprite_component.h"
#include "../../../engine/core/context.h"
#include "../../../engine/input/input_manager.h"
#include "../player_component.h"
#include "climb_state.h"
#include "fall_state.h"
#include "idle_state.h"
#include "jump_state.h"
#include "walk_state.h"

namespace game::component::state {

void ClimbState::Enter() {
  spdlog::debug("进入攀爬状态");
  PlayAnimation("climb");
  if (auto* physics = player_component_->GetPhysicsComponent(); physics) {
    physics->SetUseGravity(false);  // 禁用重力
  }
}

void ClimbState::Exit() {
  spdlog::debug("退出攀爬状态");

  if (auto* physics = player_component_->GetPhysicsComponent(); physics) {
    physics->SetUseGravity(true);  // 重新启用重力
  }
}

std::unique_ptr<PlayerState> ClimbState::HandleInput(
    engine::core::Context& context) {
  auto input_manager = context.GetInputManager();
  auto physics_component = player_component_->GetPhysicsComponent();
  auto animation_component = player_component_->GetAnimationComponent();

  // --- 攀爬状态下，按键则移动，不按键则静止 ---
  auto is_up = input_manager.IsActionDown("move_up");
  auto is_down = input_manager.IsActionDown("move_down");
  auto is_left = input_manager.IsActionDown("move_left");
  auto is_right = input_manager.IsActionDown("move_right");
  auto speed = player_component_->getClimbSpeed();

  physics_component->velocity_.y = is_up ? -speed
                                         :  // 三目运算符嵌套，自左向右执行
                                       is_down ? speed
                                               : 0.0f;
  physics_component->velocity_.x = is_left ? -speed : is_right ? speed : 0.0f;

  // --- 根据是否有按键决定动画播放情况 ---
  (is_up || is_down || is_left || is_right)
      ? animation_component->resumeAnimation()  // 有按键则恢复动画播放
      : animation_component->StopAnimation();   // 无按键则停止动画播放

  // 按跳跃键主动离开攀爬状态
  if (input_manager.IsActionPressed("jump")) {
    return std::make_unique<JumpState>(player_component_);
  }
  return nullptr;
}

std::unique_ptr<PlayerState> ClimbState::Update(float, engine::core::Context&) {
  auto physics_component = player_component_->GetPhysicsComponent();
  // 如果着地，则切换到 IdleState
  if (physics_component->HasCollidedBelow()) {
    return std::make_unique<IdleState>(player_component_);
  }
  // 如果离开梯子区域，则切换到 FallState（能走到这里 说明非着地状态）
  if (!physics_component->HasCollidedLadder()) {
    return std::make_unique<FallState>(player_component_);
  }
  return nullptr;
}

}  // namespace game::component::state
