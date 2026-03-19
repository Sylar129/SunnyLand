// Copyright Sylar129

#pragma once

#include <memory>
#include <string>

#include "engine/ecs/systems.h"
#include "engine/scene/scene.h"
#include "entt/entity/fwd.hpp"
#include "game/ecs/systems.h"
#include "glm/glm.hpp"

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
  void UpdateCamera();

  void HandlePlayerDamage(int damage);
  void ProcessRuleEvents(const game::ecs::RuleEvents& events);

  void CreateEffect(const glm::vec2& center_pos, const std::string& tag);

  void ToNextLevel(const std::string& level_name);
  void ShowEndScene(bool is_win);

  std::string LevelNameToPath(const std::string& level_name) const {
    return "assets/maps/" + level_name + ".tmj";
  }

  void CreateScoreUI();
  void CreateHealthUI();
  void AddScoreWithUI(int score);
  void HealWithUI(int amount);
  void UpdateHealthWithUI();

  entt::entity player_entity_ = entt::null;
  std::shared_ptr<game::data::Session> game_session_;

  engine::ecs::AnimationSystem animation_system_;
  engine::ecs::RenderSystem render_system_;
  engine::ecs::PhysicsSystem physics_system_;
  game::ecs::PlayerSystem player_system_;
  game::ecs::AiSystem ai_system_;
  game::ecs::GameRuleSystem game_rule_system_;

  engine::ui::UILabel* score_label_ = nullptr;
  engine::ui::UIPanel* health_panel_ = nullptr;
};

}  // namespace game::scene
