// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/scene/scene.h"

namespace engine::object {
class GameObject;
}

namespace game::scene {

class GameScene final : public engine::scene::Scene {
 public:
  GameScene(std::string name, engine::core::Context& context,
            engine::scene::SceneManager& scene_manager);

  void Init() override;
  void Update(float delta_time) override;
  void Render() override;
  void HandleInput() override;
  void Clean() override;

 private:
  // 测试函数
  void createTestObject();
};

}  // namespace game::scene
