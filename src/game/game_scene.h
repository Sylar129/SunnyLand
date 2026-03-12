// Copyright Sylar129

#pragma once

#include "engine/scene/scene.h"
#include "glm/glm.hpp"

namespace engine::object {
class GameObject;
}

namespace game::scene {

class GameScene final : public engine::scene::Scene {
 public:
  GameScene(const std::string& name, engine::core::Context& context,
            engine::scene::SceneManager& scene_manager);

  void Init() override;
  void Update(float delta_time) override;
  void Render() override;
  void HandleInput() override;
  void Clean() override;

 private:
  void InitLevel();
  void InitPlayer();
  void InitEnemyAndItem();

  void
  handleObjectCollisions();  ///< @brief
                             ///< 处理游戏对象间的碰撞逻辑（从PhysicsEngine获取信息）
  void
  handleTileTriggers();  ///< @brief 处理瓦片触发事件（从PhysicsEngine获取信息）
  void PlayerVSEnemyCollision(
      engine::object::GameObject* player,
      engine::object::GameObject* enemy);  ///< @brief 玩家与敌人碰撞处理
  void PlayerVSItemCollision(
      engine::object::GameObject* player,
      engine::object::GameObject* item);  ///< @brief 玩家与道具碰撞处理

  /**
   * @brief 创建一个特效对象（一次性）。
   * @param center_pos 特效中心位置
   * @param tag 特效标签（决定特效类型,例如"enemy","item"）
   */
  void createEffect(const glm::vec2& center_pos, const std::string& tag);

  engine::object::GameObject* player_ = nullptr;
};

}  // namespace game::scene
