// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/scene/scene.h"

namespace engine::object {
class GameObject;
}

namespace engine::scene {
class SceneManager;
}

namespace game::data {
class Session;
}

namespace game::scene {

class TitleScene final : public engine::scene::Scene {


 public:
  TitleScene(engine::core::Context& context,
             engine::scene::SceneManager& scene_manager,
             std::shared_ptr<game::data::Session> session_data = nullptr);

  ~TitleScene() override = default;

  void Init() override;
  void Update(float delta_time) override;

 private:
  void createUI();

  void onStartGameClick();
  void onLoadGameClick();
  void onHelpsClick();
  void onQuitClick();

 private:
  std::shared_ptr<game::data::Session> session_data_;
};

}  // namespace game::scene
