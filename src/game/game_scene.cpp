// Copyright Sylar129

#include "game/game_scene.h"

#include "engine/component/sprite_component.h"
#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/object/game_object.h"
#include "log.h"

namespace game::scene {

GameScene::GameScene(const std::string& name, engine::core::Context& context,
                     engine::scene::SceneManager& scene_manager)
    : Scene(name, context, scene_manager) {
  GAME_TRACE("Contructing GameScene '{}'", name);
}

void GameScene::Init() {
  CreateTestObject();

  Scene::Init();
  GAME_TRACE("Init GameScene '{}'", GetName());
}

void GameScene::Update(float delta_time) { Scene::Update(delta_time); }

void GameScene::Render() { Scene::Render(); }

void GameScene::HandleInput() { Scene::HandleInput(); }

void GameScene::Clean() { Scene::Clean(); }

void GameScene::CreateTestObject() {
  GAME_TRACE("Creating test object in GameScene");
  auto test_object =
      std::make_unique<engine::object::GameObject>("test_object");

  test_object->AddComponent<engine::component::TransformComponent>(
      glm::vec2(100.0f, 100.0f));
  test_object->AddComponent<engine::component::SpriteComponent>(
      "assets/textures/Props/big-crate.png", context_.getResourceManager());

  AddGameObject(std::move(test_object));
  GAME_TRACE("test_object has been created and added to GameScene");
}

}  // namespace game::scene
