// Copyright Sylar129

#include "game/game_scene.h"

#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "engine/object/game_object.h"
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

  CreateTestObject();
  Scene::Init();
  GAME_TRACE("Init GameScene '{}'", GetName());
}

void GameScene::Update(float delta_time) { Scene::Update(delta_time); }

void GameScene::Render() { Scene::Render(); }

void GameScene::HandleInput() {
  Scene::HandleInput();
  TestObject();
}

void GameScene::Clean() { Scene::Clean(); }

void GameScene::CreateTestObject() {
  auto test_object =
      std::make_unique<engine::object::GameObject>("test_object");
  test_object_ = test_object.get();

  test_object->AddComponent<engine::component::TransformComponent>(
      glm::vec2(100.0f, 100.0f));
  test_object->AddComponent<engine::component::SpriteComponent>(
      "assets/textures/Props/big-crate.png", context_.getResourceManager());

  test_object->AddComponent<engine::component::PhysicsComponent>(
      &context_.getPhysicsEngine());

  AddGameObject(std::move(test_object));
}

void GameScene::TestObject() {
  if (!test_object_) return;
  auto& input_manager = context_.getInputManager();
  auto* physics_comp =
      test_object_->GetComponent<engine::component::PhysicsComponent>();
  if (!physics_comp) return;

  if (input_manager.IsActionDown("move_left")) {
    test_object_->GetComponent<engine::component::TransformComponent>()
        ->Translate(glm::vec2(-2, 0));
  }
  if (input_manager.IsActionDown("move_right")) {
    test_object_->GetComponent<engine::component::TransformComponent>()
        ->Translate(glm::vec2(2, 0));
  }

  if (input_manager.IsActionPressed("jump")) {
    physics_comp->SetVelocity(glm::vec2(physics_comp->GetVelocity().x, -400));
  }
}

}  // namespace game::scene
