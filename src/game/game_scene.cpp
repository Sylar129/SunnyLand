// Copyright Sylar129

#include "game/game_scene.h"

#include "engine/component/collider_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/tilelayer_component.h"
#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "engine/object/game_object.h"
#include "engine/physics/physics_engine.h"
#include "engine/render/camera.h"
#include "engine/scene/level_loader.h"
#include "engine/utils/assert.h"
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

  auto* main_layer_obj = FindGameObjectByName("main");
  if (main_layer_obj) {
    auto* tile_layer_comp =
        main_layer_obj->GetComponent<engine::component::TileLayerComponent>();
    if (tile_layer_comp) {
      context_.GetPhysicsEngine().RegisterCollisionLayer(tile_layer_comp);
      GAME_INFO("'main' layer registered to PhysicsEngine for collisions.");
    }
  }

  player_ = FindGameObjectByName("player");
  GAME_ASSERT(player_, "No Player!");

  auto* player_transform =
      player_->GetComponent<engine::component::TransformComponent>();
  if (player_transform) {
    context_.GetCamera().SetTarget(player_transform);
  }

  auto world_size =
      main_layer_obj->GetComponent<engine::component::TileLayerComponent>()
          ->GetWorldSize();
  context_.GetCamera().SetLimitBounds(
      engine::utils::Rect(glm::vec2(0.0f), world_size));

  context_.GetPhysicsEngine().setWorldBounds(
      engine::utils::Rect(glm::vec2(0.0f), world_size));

  Scene::Init();
  GAME_TRACE("Init GameScene '{}'", GetName());
}

void GameScene::Update(float delta_time) {
  Scene::Update(delta_time);
  TestCollisionPairs();
}

void GameScene::Render() { Scene::Render(); }

void GameScene::HandleInput() {
  Scene::HandleInput();
  TestPlayer();
}

void GameScene::Clean() { Scene::Clean(); }

void GameScene::TestPlayer() {
  if (!player_) return;
  auto& input_manager = context_.GetInputManager();
  auto* pc = player_->GetComponent<engine::component::PhysicsComponent>();
  if (!pc) return;

  if (input_manager.IsActionDown("move_left")) {
    pc->velocity_.x = -100.0f;
  } else if (input_manager.IsActionDown("move_right")) {
    pc->velocity_.x = 100.0f;
  } else {
    pc->velocity_.x *= 0.9f;
  }

  if (input_manager.IsActionPressed("jump")) {
    pc->velocity_.y = -400.0f;
  }
}

void GameScene::TestCollisionPairs() {
  auto& collision_pairs = context_.GetPhysicsEngine().GetCollisionPairs();
  for (auto& pair : collision_pairs) {
    GAME_INFO("Collision between: {} and {}", pair.first->GetName(),
              pair.second->GetName());
  }
}

}  // namespace game::scene
