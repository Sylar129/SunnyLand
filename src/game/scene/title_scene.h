// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/ecs/systems.h"
#include "engine/scene/scene.h"

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
  void Render() override;

 private:
  void CreateUI();

  void OnStartGameClick();
  void OnLoadGameClick();
  void OnHelpsClick();
  void OnQuitClick();

  std::shared_ptr<game::data::Session> session_data_;
  engine::ecs::AnimationSystem animation_system_;
  engine::ecs::RenderSystem render_system_;
};

}  // namespace game::scene
