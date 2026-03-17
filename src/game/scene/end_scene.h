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

class EndScene final : public engine::scene::Scene {
 public:
  EndScene(engine::core::Context& context,
           engine::scene::SceneManager& scene_manager,
           std::shared_ptr<game::data::Session> session_data);

  ~EndScene() override = default;

  void Init() override;

 private:
  void CreateUI();
  void OnBackClick();
  void OnRestartClick();

 private:
  std::shared_ptr<game::data::Session> session_data_;
};

}  // namespace game::scene
