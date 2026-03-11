// Copyright Sylar129

#include "game/game_scene.h"

#include "engine/component/animation_component.h"
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
#include "game/component/player_component.h"
#include "log.h"

namespace game::scene {

GameScene::GameScene(const std::string& name, engine::core::Context& context,
                     engine::scene::SceneManager& scene_manager)
    : Scene(name, context, scene_manager) {
  GAME_TRACE("Contructing GameScene '{}'", name);
}

void GameScene::Init() {
  InitLevel();
  InitPlayer();
  InitEnemyAndItem();
  Scene::Init();
  GAME_TRACE("Init GameScene '{}'", GetName());
}

void GameScene::Update(float delta_time) { Scene::Update(delta_time); }

void GameScene::Render() { Scene::Render(); }

void GameScene::HandleInput() {
  Scene::HandleInput();
  TestHealth();
}

void GameScene::Clean() { Scene::Clean(); }

void GameScene::InitLevel() {
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

  auto world_size =
      main_layer_obj->GetComponent<engine::component::TileLayerComponent>()
          ->GetWorldSize();
  context_.GetCamera().SetLimitBounds(
      engine::utils::Rect(glm::vec2(0.0f), world_size));

  context_.GetPhysicsEngine().SetWorldBounds(
      engine::utils::Rect(glm::vec2(0.0f), world_size));
}

void GameScene::InitPlayer() {
  player_ = FindGameObjectByName("player");
  GAME_ASSERT(player_, "No Player!");

  auto* player_component =
      player_->AddComponent<game::component::PlayerComponent>();

  GAME_ASSERT(player_component,
              "Failed to add PlayerComponent to player object");

  auto* player_transform =
      player_->GetComponent<engine::component::TransformComponent>();
  if (player_transform) {
    context_.GetCamera().SetTarget(player_transform);
  }
}

void GameScene::InitEnemyAndItem() {
  for (auto& game_object : game_objects_) {
    if (game_object->GetName() == "eagle") {
      if (auto* ac =
              game_object
                  ->GetComponent<engine::component::AnimationComponent>();
          ac) {
        ac->PlayAnimation("fly");
      } else {
        GAME_ERROR(
            "Eagle object missing AnimationComponent, cannot play animation.");
      }
    }
    if (game_object->GetName() == "frog") {
      if (auto* ac =
              game_object
                  ->GetComponent<engine::component::AnimationComponent>();
          ac) {
        ac->PlayAnimation("idle");
      } else {
        GAME_ERROR(
            "Frog object missing AnimationComponent, cannot play animation.");
      }
    }
    if (game_object->GetName() == "opossum") {
      if (auto* ac =
              game_object
                  ->GetComponent<engine::component::AnimationComponent>();
          ac) {
        ac->PlayAnimation("walk");
      } else {
        GAME_ERROR(
            "Opossum object missing AnimationComponent, cannot play "
            "animation.");
      }
    }
    if (game_object->GetTag() == "item") {
      if (auto* ac =
              game_object
                  ->GetComponent<engine::component::AnimationComponent>();
          ac) {
        ac->PlayAnimation("idle");
      } else {
        GAME_ERROR(
            "Item object missing AnimationComponent, cannot play animation.");
      }
    }
  }
}

void GameScene::TestHealth() {
  auto input_manager = context_.GetInputManager();
  if (input_manager.IsActionPressed("attack")) {
    player_->GetComponent<game::component::PlayerComponent>()->TakeDamage(1);
  }
}

}  // namespace game::scene
