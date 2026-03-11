#include "dead_state.h"

#include "../../../engine/component/collider_component.h"
#include "../../../engine/object/game_object.h"
#include "../player_component.h"
#include "engine/component/physics_component.h"
#include "log.h"
//...

namespace game::component::state {

void DeadState::Enter() {
  GAME_DEBUG("玩家进入死亡状态。");
  PlayAnimation("hurt");  // 播放死亡(受伤)动画

  // 应用击退力（只向上）
  auto physics_component = player_component_->GetPhysicsComponent();
  physics_component->velocity_ = glm::vec2(0.0f, -200.0f);  // 向上弹起

  // 禁用碰撞(使其可以掉出屏幕)
  auto collider_component =
      player_component_->GetOwner()
          ->GetComponent<engine::component::ColliderComponent>();
  if (collider_component) {
    collider_component->SetActive(false);
  }
}

void DeadState::Exit() {}
//...
std::unique_ptr<PlayerState> DeadState::HandleInput(engine::core::Context&) {
  // 死亡状态下不处理输入
  return nullptr;
}

std::unique_ptr<PlayerState> DeadState::Update(float, engine::core::Context&) {
  // 死亡状态下不更新状态
  return nullptr;
}
}  // namespace game::component::state
