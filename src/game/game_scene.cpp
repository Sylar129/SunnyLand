// Copyright Sylar129

#include "game/game_scene.h"

#include "engine/component/sprite_component.h"
#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "engine/object/game_object.h"
#include "engine/render/camera.h"
#include "engine/scene/level_loader.h"
#include "log.h"

namespace game::scene {

GameScene::GameScene(const std::string& name, engine::core::Context& context,
                     engine::scene::SceneManager& scene_manager)
    : Scene(name, context, scene_manager) {
  GAME_TRACE("Contructing GameScene '{}'", name);
}

void GameScene::Init() {
  engine::scene::LevelLoader level_loader;
  level_loader.LoadLevel("assets/maps/level1.tmj", *this);

  Scene::Init();
  GAME_TRACE("Init GameScene '{}'", GetName());
}

void GameScene::Update(float delta_time) { Scene::Update(delta_time); }

void GameScene::Render() { Scene::Render(); }

void GameScene::HandleInput() {
  Scene::HandleInput();
  TestCamera();
}

void GameScene::Clean() { Scene::Clean(); }

void GameScene::TestCamera() {
  auto& camera = context_.getCamera();
  auto& input_manager = context_.getInputManager();
  if (input_manager.IsActionDown("move_up")) camera.Move(glm::vec2(0, -2));
  if (input_manager.IsActionDown("move_down")) camera.Move(glm::vec2(0, 2));
  if (input_manager.IsActionDown("move_left")) camera.Move(glm::vec2(-2, 0));
  if (input_manager.IsActionDown("move_right")) camera.Move(glm::vec2(2, 0));
}

}  // namespace game::scene
