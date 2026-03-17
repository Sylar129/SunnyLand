// Copyright Sylar129

#include "game/scene/helps_scene.h"

#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "engine/scene/scene_manager.h"
#include "engine/ui/ui_image.h"
#include "engine/ui/ui_manager.h"
#include "utils/log.h"

namespace game::scene {

HelpsScene::HelpsScene(engine::core::Context& context,
                       engine::scene::SceneManager& scene_manager)
    : engine::scene::Scene("HelpsScene", context, scene_manager) {
  GAME_LOG_TRACE("HelpsScene constructed");
}

void HelpsScene::Init() {
  if (is_initialized_) {
    return;
  }
  auto window_size = glm::vec2(640.0f, 360.0f);

  auto help_image = std::make_unique<engine::ui::UIImage>(
      "assets/textures/UI/instructions.png", glm::vec2(0.0f, 0.0f),
      window_size);

  ui_manager_->AddElement(std::move(help_image));

  Scene::Init();
  GAME_LOG_TRACE("HelpsScene initialized");
}

void HelpsScene::HandleInput() {
  if (!is_initialized_) return;

  if (context_.GetInputManager().IsActionPressed("MouseLeftClick")) {
    GAME_LOG_DEBUG("Mouse left click detected, popping HelpsScene.");
    scene_manager_.RequestPopScene();
  }
}

}  // namespace game::scene
