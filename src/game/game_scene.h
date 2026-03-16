// Copyright Sylar129

#pragma once

#include "engine/scene/scene.h"
#include "glm/glm.hpp"

namespace engine::object {
class GameObject;
}

namespace game::data {
class Session;
}

namespace engine::ui {
class UILabel;
class UIPanel;
}  // namespace engine::ui

namespace game::scene {

class GameScene final : public engine::scene::Scene {
 public:
  GameScene(engine::core::Context& context,
            engine::scene::SceneManager& scene_manager,
            std::shared_ptr<game::data::Session> session = nullptr);

  void Init() override;
  void Update(float delta_time) override;
  void Render() override;
  void HandleInput() override;
  void Clean() override;

 private:
  void InitLevel();
  void InitPlayer();
  void InitEnemyAndItem();
  void InitUI();

  void HandleObjectCollisions();
  void HandleTileTriggers();
  void HandlePlayerDamage(int damage);
  void PlayerVSEnemyCollision(engine::object::GameObject* player,
                              engine::object::GameObject* enemy);
  void PlayerVSItemCollision(engine::object::GameObject* player,
                             engine::object::GameObject* item);

  void CreateEffect(const glm::vec2& center_pos, const std::string& tag);

  void ToNextLevel(engine::object::GameObject* trigger);

  std::string LevelNameToPath(const std::string& level_name) const {
    return "assets/maps/" + level_name + ".tmj";
  }

  // --- UI 相关函数 ---
  void createScoreUI();   ///< @brief 创建得分UI
  void createHealthUI();  ///< @brief 创建生命值UI (或最大生命值改变时重设)
  void addScoreWithUI(int score);  ///< @brief 增加得分，同时更新UI
  void healWithUI(int amount);     ///< @brief 增加生命，同时更新UI
  void
  updateHealthWithUI();  ///< @brief 更新生命值UI (只适用最大生命值不变的情况)

  engine::object::GameObject* player_ = nullptr;
  std::shared_ptr<game::data::Session> game_session_;

  engine::ui::UILabel* score_label_ =
      nullptr;  ///< @brief 得分标签 (生命周期由UIManager管理，因此使用裸指针)
  engine::ui::UIPanel* health_panel_ = nullptr;  ///< @brief 生命值图标面板
};

}  // namespace game::scene
