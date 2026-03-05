// Copyright Sylar129

#include "game/game_scene.h"

#include <SDL3/SDL_rect.h>
#include <spdlog/spdlog.h>

#include "engine/component/sprite_component.h"
#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/object/game_object.h"

namespace game::scene {

GameScene::GameScene(std::string name, engine::core::Context& context,
                     engine::scene::SceneManager& scene_manager)
    : Scene(name, context, scene_manager) {
  spdlog::trace("GameScene 构造完成。");
}

void GameScene::Init() {
  // 创建 test_object
  createTestObject();

  Scene::Init();
  spdlog::trace("GameScene 初始化完成。");
}

void GameScene::Update(float delta_time) { Scene::Update(delta_time); }

void GameScene::Render() { Scene::Render(); }

void GameScene::HandleInput() { Scene::HandleInput(); }

void GameScene::Clean() { Scene::Clean(); }

// --- 私有方法 ---

void GameScene::createTestObject() {
  spdlog::trace("在 GameScene 中创建 test_object...");
  auto test_object =
      std::make_unique<engine::object::GameObject>("test_object");

  // 添加组件
  test_object->AddComponent<engine::component::TransformComponent>(
      glm::vec2(100.0f, 100.0f));
  test_object->AddComponent<engine::component::SpriteComponent>(
      "assets/textures/Props/big-crate.png", context_.getResourceManager());

  // 将创建好的 GameObject 添加到场景中 （一定要用std::move，否则传递的是左值）
  AddGameObject(std::move(test_object));
  spdlog::trace("test_object 创建并添加到 GameScene 中。");
}

}  // namespace game::scene
