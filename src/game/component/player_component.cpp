// Copyright Sylar129

#include "game/component/player_component.h"

#include <typeinfo>
#include <utility>

#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/transform_component.h"
#include "engine/object/game_object.h"
#include "game/component/state/idle_state.h"
#include "log.h"

namespace game::component {

void PlayerComponent::Init() {
  if (!owner_) {
    GAME_ERROR("PlayerComponent 没有所属游戏对象!");
    return;
  }

  // 获取必要的组件
  transform_component_ =
      owner_->GetComponent<engine::component::TransformComponent>();
  physics_component_ =
      owner_->GetComponent<engine::component::PhysicsComponent>();
  sprite_component_ =
      owner_->GetComponent<engine::component::SpriteComponent>();

  // 检查必要组件是否存在
  if (!transform_component_ || !physics_component_ || !sprite_component_) {
    GAME_ERROR("Player 对象缺少必要组件！");
  }

  // 初始化状态机
  current_state_ = std::make_unique<state::IdleState>(this);
  if (current_state_) {
    setState(std::move(current_state_));
  } else {
    GAME_ERROR("初始化玩家状态失败（make_unique 返回空指针）！");
  }
  GAME_DEBUG("PlayerComponent 初始化完成。");
}

void PlayerComponent::setState(std::unique_ptr<state::PlayerState> new_state) {
  if (!new_state) {
    GAME_WARN("尝试设置空的玩家状态！");
    return;
  }
  if (current_state_) {
    current_state_->Exit();
  }

  current_state_ = std::move(new_state);
  GAME_DEBUG("玩家组件正在切换到状态: {}", typeid(*current_state_).name());
  current_state_->Enter();
}

void PlayerComponent::HandleInput(engine::core::Context& context) {
  if (!current_state_) return;

  auto next_state = current_state_->HandleInput(context);
  if (next_state) {
    setState(std::move(next_state));
  }
}

void PlayerComponent::Update(float delta_time, engine::core::Context& context) {
  if (!current_state_) return;

  auto next_state = current_state_->Update(delta_time, context);
  if (next_state) {
    setState(std::move(next_state));
  }
}

}  // namespace game::component
