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

  void Test();

  engine::object::GameObject* player_ = nullptr;
  std::shared_ptr<game::data::Session> game_session_;
};

}  // namespace game::scene
