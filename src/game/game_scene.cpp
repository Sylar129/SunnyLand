// Copyright Sylar129

#include "game/game_scene.h"

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
  handleObjectCollisions();
  handleTileTriggers();
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

void GameScene::handleObjectCollisions() {
  // 从物理引擎中获取碰撞对
  auto collision_pairs = context_.GetPhysicsEngine().GetCollisionPairs();
  for (const auto& pair : collision_pairs) {
    auto* obj1 = pair.first;
    auto* obj2 = pair.second;

    // 处理玩家与敌人的碰撞
    if (obj1->GetName() == "player" && obj2->GetTag() == "enemy") {
      PlayerVSEnemyCollision(obj1, obj2);
    } else if (obj2->GetName() == "player" && obj1->GetTag() == "enemy") {
      PlayerVSEnemyCollision(obj2, obj1);
    }
    // 处理玩家与道具的碰撞
    else if (obj1->GetName() == "player" && obj2->GetTag() == "item") {
      PlayerVSItemCollision(obj1, obj2);
    } else if (obj2->GetName() == "player" && obj1->GetTag() == "item") {
      PlayerVSItemCollision(obj2, obj1);
    }  // 处理玩家与"hazard"对象碰撞
    else if (obj1->GetName() == "player" && obj2->GetTag() == "hazard") {
      obj1->GetComponent<game::component::PlayerComponent>()->TakeDamage(1);
      GAME_DEBUG("玩家 {} 受到了 HAZARD 对象伤害", obj1->GetName());
    } else if (obj2->GetName() == "player" && obj1->GetTag() == "hazard") {
      obj2->GetComponent<game::component::PlayerComponent>()->TakeDamage(1);
      GAME_DEBUG("玩家 {} 受到了 HAZARD 对象伤害", obj2->GetName());
    }
  }
}

void GameScene::handleTileTriggers() {
  const auto& tile_trigger_events =
      context_.GetPhysicsEngine().getTileTriggerEvents();
  for (const auto& event : tile_trigger_events) {
    auto* obj = event.first;        // 瓦片触发事件的对象
    auto tile_type = event.second;  // 瓦片类型
    if (tile_type == engine::component::TileType::HAZARD) {
      // 玩家碰到到危险瓦片，受伤
      if (obj->GetName() == "player") {
        obj->GetComponent<game::component::PlayerComponent>()->TakeDamage(1);
        GAME_DEBUG("玩家 {} 受到了 HAZARD 瓦片伤害", obj->GetName());
      }
      // TODO: 其他对象类型的处理，目前让敌人无视瓦片伤害
    }
  }
}

void GameScene::PlayerVSEnemyCollision(engine::object::GameObject* player,
                                       engine::object::GameObject* enemy) {
  // --- 踩踏判断逻辑：1. 玩家中心点在敌人上方    2. 重叠区域：overlap.x >
  // overlap.y
  auto player_aabb =
      player->GetComponent<engine::component::ColliderComponent>()
          ->GetWorldAABB();
  auto enemy_aabb = enemy->GetComponent<engine::component::ColliderComponent>()
                        ->GetWorldAABB();
  auto player_center = player_aabb.position + player_aabb.size / 2.0f;
  auto enemy_center = enemy_aabb.position + enemy_aabb.size / 2.0f;
  auto overlap = glm::vec2(player_aabb.size / 2.0f + enemy_aabb.size / 2.0f) -
                 glm::abs(player_center - enemy_center);

  // 踩踏判断成功，敌人受伤
  if (overlap.x > overlap.y && player_center.y < enemy_center.y) {
    GAME_INFO("玩家 {} 踩踏了敌人 {}", player->GetName(), enemy->GetName());
    auto enemy_health =
        enemy->GetComponent<engine::component::HealthComponent>();
    if (!enemy_health) {
      GAME_ERROR("敌人 {} 没有 HealthComponent 组件，无法处理踩踏伤害",
                 enemy->GetName());
      return;
    }
    enemy_health->TakeDamage(1);  // 造成1点伤害
    if (!enemy_health->IsAlive()) {
      GAME_INFO("敌人 {} 被踩踏后死亡", enemy->GetName());
      enemy->SetNeedRemove(true);                   // 标记敌人为待删除状态
      createEffect(enemy_center, enemy->GetTag());  // 创建（死亡）特效
    }
    // 玩家跳起效果
    player->GetComponent<engine::component::PhysicsComponent>()->velocity_.y =
        -300.0f;  // 向上跳起
  }
  // 踩踏判断失败，玩家受伤
  else {
    GAME_INFO("敌人 {} 对玩家 {} 造成伤害", enemy->GetName(),
              player->GetName());
    player->GetComponent<game::component::PlayerComponent>()->TakeDamage(1);
    // TODO: 其他受伤逻辑
  }
}

void GameScene::PlayerVSItemCollision(engine::object::GameObject* player,
                                      engine::object::GameObject* item) {
  if (item->GetName() == "fruit") {
    player->GetComponent<engine::component::HealthComponent>()->Heal(
        1);  // 加血
  } else if (item->GetName() == "gem") {
    // TODO: 加分
  }
  item->SetNeedRemove(true);  // 标记道具为待删除状态
  auto item_aabb = item->GetComponent<engine::component::ColliderComponent>()
                       ->GetWorldAABB();
  createEffect(item_aabb.position + item_aabb.size / 2.0f,
               item->GetTag());  // 创建特效
}

void GameScene::createEffect(const glm::vec2& center_pos,
                             const std::string& tag) {
  // --- 创建游戏对象和变换组件 ---
  auto effect_obj =
      std::make_unique<engine::object::GameObject>("effect_" + tag);
  effect_obj->AddComponent<engine::component::TransformComponent>(center_pos);

  // --- 根据标签创建不同的精灵组件和动画---
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
    GAME_WARN("未知特效类型: {}", tag);
    return;
  }

  // --- 根据创建的动画，添加动画组件，并设置为单次播放 ---
  auto* animation_component =
      effect_obj->AddComponent<engine::component::AnimationComponent>();
  animation_component->AddAnimation(std::move(animation));
  animation_component->SetOneShotRemoval(true);
  animation_component->PlayAnimation("effect");
  SafeAddGameObject(std::move(effect_obj));  // 安全添加特效对象
  GAME_DEBUG("创建特效: {}", tag);
}

}  // namespace game::scene
