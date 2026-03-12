// Copyright Sylar129

#include "game/game_scene.h"

#include "engine/component/ai/jump_behavior.h"
#include "engine/component/ai/patrol_behavior.h"
#include "engine/component/ai/updown_behavior.h"
#include "engine/component/ai_component.h"
#include "engine/component/animation_component.h"
#include "engine/component/collider_component.h"
#include "engine/component/health_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/tilelayer_component.h"
#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/input/input_manager.h"
#include "engine/object/game_object.h"
#include "engine/physics/physics_engine.h"
#include "engine/render/animation.h"
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

void GameScene::Update(float delta_time) {
  Scene::Update(delta_time);
  HandleObjectCollisions();
  HandleTileTriggers();
}

void GameScene::Render() { Scene::Render(); }

void GameScene::HandleInput() { Scene::HandleInput(); }

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
      if (auto* ai_component =
              game_object->AddComponent<game::component::AIComponent>();
          ai_component) {
        auto y_max =
            game_object->GetComponent<engine::component::TransformComponent>()
                ->GetPosition()
                .y;
        auto y_min = y_max - 80.0f;
        ai_component->setBehavior(
            std::make_unique<game::component::ai::UpDownBehavior>(y_min,
                                                                  y_max));
      }
    }
    if (game_object->GetName() == "frog") {
      if (auto* ai_component =
              game_object->AddComponent<game::component::AIComponent>();
          ai_component) {
        auto x_max =
            game_object->GetComponent<engine::component::TransformComponent>()
                ->GetPosition()
                .x -
            10.0f;
        auto x_min = x_max - 90.0f;
        ai_component->setBehavior(
            std::make_unique<game::component::ai::JumpBehavior>(x_min, x_max));
      }
    }
    if (game_object->GetName() == "opossum") {
      if (auto* ai_component =
              game_object->AddComponent<game::component::AIComponent>();
          ai_component) {
        auto x_max =
            game_object->GetComponent<engine::component::TransformComponent>()
                ->GetPosition()
                .x;
        auto x_min = x_max - 200.0f;
        ai_component->setBehavior(
            std::make_unique<game::component::ai::PatrolBehavior>(x_min,
                                                                  x_max));
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

void GameScene::HandleObjectCollisions() {
  auto collision_pairs = context_.GetPhysicsEngine().GetCollisionPairs();
  for (const auto& pair : collision_pairs) {
    auto* obj1 = pair.first;
    auto* obj2 = pair.second;

    if (obj1->GetName() == "player" && obj2->GetTag() == "enemy") {
      PlayerVSEnemyCollision(obj1, obj2);
    } else if (obj2->GetName() == "player" && obj1->GetTag() == "enemy") {
      PlayerVSEnemyCollision(obj2, obj1);
    } else if (obj1->GetName() == "player" && obj2->GetTag() == "item") {
      PlayerVSItemCollision(obj1, obj2);
    } else if (obj2->GetName() == "player" && obj1->GetTag() == "item") {
      PlayerVSItemCollision(obj2, obj1);
    } else if (obj1->GetName() == "player" && obj2->GetTag() == "hazard") {
      obj1->GetComponent<game::component::PlayerComponent>()->TakeDamage(1);
      GAME_DEBUG("Player '{}' taking damage from hazard", obj1->GetName());
    } else if (obj2->GetName() == "player" && obj1->GetTag() == "hazard") {
      obj2->GetComponent<game::component::PlayerComponent>()->TakeDamage(1);
      GAME_DEBUG("Player '{}' taking damage from hazard", obj2->GetName());
    }
  }
}

void GameScene::HandleTileTriggers() {
  const auto& tile_trigger_events =
      context_.GetPhysicsEngine().getTileTriggerEvents();
  for (const auto& event : tile_trigger_events) {
    auto* obj = event.first;
    auto tile_type = event.second;
    if (tile_type == engine::component::TileType::HAZARD) {
      if (obj->GetName() == "player") {
        obj->GetComponent<game::component::PlayerComponent>()->TakeDamage(1);
        GAME_DEBUG("Player '{}' taking damage from hazard", obj->GetName());
      }
    }
  }
}

void GameScene::PlayerVSEnemyCollision(engine::object::GameObject* player,
                                       engine::object::GameObject* enemy) {
  auto player_aabb =
      player->GetComponent<engine::component::ColliderComponent>()
          ->GetWorldAABB();
  auto enemy_aabb = enemy->GetComponent<engine::component::ColliderComponent>()
                        ->GetWorldAABB();
  auto player_center = player_aabb.position + player_aabb.size / 2.0f;
  auto enemy_center = enemy_aabb.position + enemy_aabb.size / 2.0f;
  auto overlap = glm::vec2(player_aabb.size / 2.0f + enemy_aabb.size / 2.0f) -
                 glm::abs(player_center - enemy_center);

  if (overlap.x > overlap.y && player_center.y < enemy_center.y) {
    GAME_INFO("Player {} stomped on enemy {}", player->GetName(),
              enemy->GetName());
    auto enemy_health =
        enemy->GetComponent<engine::component::HealthComponent>();
    if (!enemy_health) {
      GAME_ERROR("Enemy {} does not have HealthComponent, cannot take damage.",
                 enemy->GetName());
      return;
    }
    enemy_health->TakeDamage(1);
    if (!enemy_health->IsAlive()) {
      GAME_INFO("Enemy {} defeated by player {}", enemy->GetName(),
                player->GetName());
      enemy->SetNeedRemove(true);
      CreateEffect(enemy_center, enemy->GetTag());
    }
    player->GetComponent<engine::component::PhysicsComponent>()->velocity_.y =
        -300.0f;
  } else {
    GAME_INFO("Enemy {} collided with player {}, causing damage",
              enemy->GetName(), player->GetName());
    player->GetComponent<game::component::PlayerComponent>()->TakeDamage(1);
  }
}

void GameScene::PlayerVSItemCollision(engine::object::GameObject* player,
                                      engine::object::GameObject* item) {
  if (item->GetName() == "fruit") {
    player->GetComponent<engine::component::HealthComponent>()->Heal(1);
  } else if (item->GetName() == "gem") {
    // TODO: add points
  }
  item->SetNeedRemove(true);
  auto item_aabb = item->GetComponent<engine::component::ColliderComponent>()
                       ->GetWorldAABB();
  CreateEffect(item_aabb.position + item_aabb.size / 2.0f, item->GetTag());
}

void GameScene::CreateEffect(const glm::vec2& center_pos,
                             const std::string& tag) {
  auto effect_obj =
      std::make_unique<engine::object::GameObject>("effect_" + tag);
  effect_obj->AddComponent<engine::component::TransformComponent>(center_pos);

  auto animation = std::make_unique<engine::render::Animation>("effect", false);
  if (tag == "enemy") {
    effect_obj->AddComponent<engine::component::SpriteComponent>(
        "assets/textures/FX/enemy-deadth.png", context_.GetResourceManager(),
        engine::utils::Alignment::CENTER);
    for (auto i = 0; i < 5; ++i) {
      animation->addFrame({static_cast<float>(i * 40), 0.0f, 40.0f, 41.0f},
                          0.1f);
    }
  } else if (tag == "item") {
    effect_obj->AddComponent<engine::component::SpriteComponent>(
        "assets/textures/FX/item-feedback.png", context_.GetResourceManager(),
        engine::utils::Alignment::CENTER);
    for (auto i = 0; i < 4; ++i) {
      animation->addFrame({static_cast<float>(i * 32), 0.0f, 32.0f, 32.0f},
                          0.1f);
    }
  } else {
    GAME_WARN("Unknown effect type: {}", tag);
    return;
  }

  auto* animation_component =
      effect_obj->AddComponent<engine::component::AnimationComponent>();
  animation_component->AddAnimation(std::move(animation));
  animation_component->SetOneShotRemoval(true);
  animation_component->PlayAnimation("effect");
  SafeAddGameObject(std::move(effect_obj));
  GAME_DEBUG("Created effect: {}", tag);
}

}  // namespace game::scene
