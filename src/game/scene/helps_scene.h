// Copyright Sylar129

#pragma once

#include "engine/scene/scene.h"

namespace engine::core {
class Context;
}
namespace engine::scene {
class SceneManager;
}

namespace game::scene {

class HelpsScene final : public engine::scene::Scene {
 public:
  HelpsScene(engine::core::Context& context,
             engine::scene::SceneManager& scene_manager);

  ~HelpsScene() override = default;

  void Init() override;
  void HandleInput() override;
};

}  // namespace game::scene
