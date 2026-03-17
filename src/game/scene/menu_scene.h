// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/scene/scene.h"

namespace engine::core {
class Context;
}
namespace engine::scene {
class SceneManager;
}
namespace game::data {
class Session;
}

namespace game::scene {

class MenuScene final : public engine::scene::Scene {
 public:
  MenuScene(engine::core::Context& context,
            engine::scene::SceneManager& scene_manager,
            std::shared_ptr<game::data::Session> session_data_);

  ~MenuScene() override = default;

  void Init() override;
  void HandleInput() override;

 private:
  void CreateUI();
  void OnResumeClicked();
  void OnSaveClicked();
  void OnBackClicked();
  void OnQuitClicked();

  std::shared_ptr<game::data::Session> session_data_;
};

}  // namespace game::scene
