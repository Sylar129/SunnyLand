// Copyright Sylar129

#include "game/scene/game_scene.h"

#include "engine/component/animation_component.h"
#include "engine/component/collider_component.h"
#include "engine/component/health_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/tilelayer_component.h"
#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/core/game_state.h"
#include "engine/input/input_manager.h"
#include "engine/object/game_object.h"
#include "engine/physics/physics_engine.h"
#include "engine/render/animation.h"
#include "engine/render/camera.h"
#include "engine/scene/level_loader.h"
#include "engine/scene/scene_manager.h"
#include "engine/ui/ui_image.h"
#include "engine/ui/ui_label.h"
#include "engine/ui/ui_manager.h"
#include "engine/ui/ui_panel.h"
#include "game/component/ai/jump_behavior.h"
#include "game/component/ai/patrol_behavior.h"
#include "game/component/ai/updown_behavior.h"
#include "game/component/ai_component.h"
#include "game/component/player_component.h"
#include "game/data/session_data.h"
#include "game/scene/end_scene.h"
#include "game/scene/menu_scene.h"
#include "utils/assert.h"
#include "utils/log.h"

namespace game::scene {

GameScene::GameScene(engine::core::Context& context,
                     engine::scene::SceneManager& scene_manager,
                     std::shared_ptr<game::data::Session> data)
    : Scene("GameScene", context, scene_manager),
      game_session_(std::move(data)) {
  if (!game_session_) {
    game_session_ = std::make_shared<game::data::Session>();
    GAME_LOG_INFO("No SessionData provided, created default SessionData.");
  }
  GAME_LOG_TRACE("GameScene constructed");
}

void GameScene::Init() {
  if (is_initialized_) {
    return;
  }
  context_.GetGameState().SetState(engine::core::State::kPlaying);
  InitLevel();
  InitPlayer();
  InitEnemyAndItem();
  InitUI();
  Scene::Init();
  GAME_LOG_TRACE("Init GameScene '{}'", GetName());
}

void GameScene::Update(float delta_time) {
  Scene::Update(delta_time);
  HandleObjectCollisions();
  HandleTileTriggers();

  if (player_) {
    auto pos = player_->GetComponent<engine::component::TransformComponent>()
                   ->GetPosition();
    auto world_rect = context_.GetPhysicsEngine().GetWorldBounds();
    if (world_rect &&
        pos.y > world_rect->position.y + world_rect->size.y + 100.0f) {
      ShowEndScene(false);
    }
  }
}

void GameScene::Render() { Scene::Render(); }

void GameScene::HandleInput() {
  Scene::HandleInput();
  if (context_.GetInputManager().IsActionPressed("pause")) {
    scene_manager_.RequestPushScene(
        std::make_unique<MenuScene>(context_, scene_manager_, game_session_));
  }
}

void GameScene::Clean() { Scene::Clean(); }

void GameScene::InitLevel() {
  engine::scene::LevelLoader level_loader;
  auto level_path = game_session_->GetCurrentMapPath();
  if (!level_loader.LoadLevel(level_path, *this)) {
    GAME_LOG_ERROR("Load level '{}' failed!", level_path);
    return;
  }

  auto* main_layer_obj = FindGameObjectByName("main");
  if (main_layer_obj) {
    auto* tile_layer_comp =
        main_layer_obj->GetComponent<engine::component::TileLayerComponent>();
    if (tile_layer_comp) {
      context_.GetPhysicsEngine().RegisterCollisionLayer(tile_layer_comp);
      GAME_LOG_INFO("'main' layer registered to PhysicsEngine for collisions.");
    }
  }

  auto world_size =
      main_layer_obj->GetComponent<engine::component::TileLayerComponent>()
          ->GetWorldSize();
  context_.GetCamera().SetLimitBounds(utils::Rect(glm::vec2(0.0f), world_size));
  context_.GetCamera().SetPosition(glm::vec2(0.0f));

  context_.GetPhysicsEngine().SetWorldBounds(
      utils::Rect(glm::vec2(0.0f), world_size));
}

void GameScene::InitPlayer() {
  player_ = FindGameObjectByName("player");
  GAME_LOG_ASSERT(player_, "No Player!");

  auto* player_component =
      player_->AddComponent<game::component::PlayerComponent>();

  GAME_LOG_ASSERT(player_component,
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
        ai_component->SetBehavior(
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
        ai_component->SetBehavior(
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
        ai_component->SetBehavior(
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
        GAME_LOG_ERROR(
            "Item object missing AnimationComponent, cannot play animation.");
      }
    }
  }
}

void GameScene::InitUI() {
  if (!ui_manager_->Init(context_.GetGameState().GetLogicalSize())) {
    GAME_LOG_ERROR("Failed to initialize UIManager.");
    return;
  }

  CreateScoreUI();
  CreateHealthUI();
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
      HandlePlayerDamage(1);
      GAME_LOG_DEBUG("Player '{}' taking damage from hazard", obj1->GetName());
    } else if (obj2->GetName() == "player" && obj1->GetTag() == "hazard") {
      HandlePlayerDamage(1);
      GAME_LOG_DEBUG("Player '{}' taking damage from hazard", obj2->GetName());
    } else if (obj1->GetName() == "player" && obj2->GetTag() == "next_level") {
      ToNextLevel(obj2);
    } else if (obj2->GetName() == "player" && obj1->GetTag() == "next_level") {
      ToNextLevel(obj1);
    } else if (obj1->GetName() == "player" && obj2->GetName() == "win") {
      ShowEndScene(true);
    } else if (obj2->GetName() == "player" && obj1->GetName() == "win") {
      ShowEndScene(true);
    }
  }
}

void GameScene::HandleTileTriggers() {
  const auto& tile_trigger_events =
      context_.GetPhysicsEngine().getTileTriggerEvents();
  for (const auto& event : tile_trigger_events) {
    auto* obj = event.first;
    auto tile_type = event.second;
    if (tile_type == "hazard") {
      if (obj->GetName() == "player") {
        HandlePlayerDamage(1);
        GAME_LOG_DEBUG("Player '{}' taking damage from hazard", obj->GetName());
      }
    }
  }
}

void GameScene::HandlePlayerDamage(int damage) {
  auto player_component =
      player_->GetComponent<game::component::PlayerComponent>();
  if (!player_component->TakeDamage(damage)) {
    return;
  }
  if (player_component->IsDead()) {
    GAME_LOG_INFO("Player {} is dead", player_->GetName());
  }
  UpdateHealthWithUI();
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
    GAME_LOG_INFO("Player {} stomped on enemy {}", player->GetName(),
                  enemy->GetName());
    auto enemy_health =
        enemy->GetComponent<engine::component::HealthComponent>();
    if (!enemy_health) {
      GAME_LOG_ERROR(
          "Enemy {} does not have HealthComponent, cannot take damage.",
          enemy->GetName());
      return;
    }
    enemy_health->TakeDamage(1);
    if (!enemy_health->IsAlive()) {
      GAME_LOG_INFO("Enemy {} defeated by player {}", enemy->GetName(),
                    player->GetName());
      enemy->SetNeedRemove(true);
      AddScoreWithUI(10);

      CreateEffect(enemy_center, enemy->GetTag());
    }
    player->GetComponent<engine::component::PhysicsComponent>()->SetVelocityY(
        -300.0f);
  } else {
    GAME_LOG_INFO("Enemy {} collided with player {}, causing damage",
                  enemy->GetName(), player->GetName());
    HandlePlayerDamage(1);
  }
}

void GameScene::PlayerVSItemCollision(engine::object::GameObject* player,
                                      engine::object::GameObject* item) {
  if (item->GetName() == "fruit") {
    HealWithUI(1);
  } else if (item->GetName() == "gem") {
    AddScoreWithUI(5);
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
        utils::Alignment::kCenter);
    for (auto i = 0; i < 5; ++i) {
      animation->AddFrame({static_cast<float>(i * 40), 0.0f, 40.0f, 41.0f},
                          0.1f);
    }
  } else if (tag == "item") {
    effect_obj->AddComponent<engine::component::SpriteComponent>(
        "assets/textures/FX/item-feedback.png", context_.GetResourceManager(),
        utils::Alignment::kCenter);
    for (auto i = 0; i < 4; ++i) {
      animation->AddFrame({static_cast<float>(i * 32), 0.0f, 32.0f, 32.0f},
                          0.1f);
    }
  } else {
    GAME_LOG_WARN("Unknown effect type: {}", tag);
    return;
  }

  auto* animation_component =
      effect_obj->AddComponent<engine::component::AnimationComponent>();
  animation_component->AddAnimation(std::move(animation));
  animation_component->SetOneShotRemoval(true);
  animation_component->PlayAnimation("effect");
  SafeAddGameObject(std::move(effect_obj));
  GAME_LOG_DEBUG("Created effect: {}", tag);
}

void GameScene::ToNextLevel(engine::object::GameObject* trigger) {
  auto scene_name = trigger->GetName();
  auto map_path = LevelNameToPath(scene_name);
  game_session_->SetNextMapPath(map_path);
  auto next_scene = std::make_unique<game::scene::GameScene>(
      context_, scene_manager_, game_session_);
  scene_manager_.RequestReplaceScene(std::move(next_scene));
}

void GameScene::ShowEndScene(bool is_win) {
  game_session_->SetIsWin(is_win);
  auto end_scene = std::make_unique<game::scene::EndScene>(
      context_, scene_manager_, game_session_);
  scene_manager_.RequestPushScene(std::move(end_scene));
}

void GameScene::CreateScoreUI() {
  auto score_text =
      "Score: " + std::to_string(game_session_->GetCurrentScore());
  auto score_label = std::make_unique<engine::ui::UILabel>(
      context_.GetTextRenderer(), score_text,
      "assets/fonts/VonwaonBitmap-16px.ttf", 16);
  score_label_ = score_label.get();
  auto screen_size = ui_manager_->GetRootElement()->GetSize();
  score_label_->SetPosition(glm::vec2(screen_size.x - 100.0f, 10.0f));
  ui_manager_->AddElement(std::move(score_label));
}

void GameScene::CreateHealthUI() {
  int max_health = game_session_->GetMaxHealth();
  int current_health = game_session_->GetCurrentHealth();
  float start_x = 10.0f;
  float start_y = 10.0f;
  float icon_width = 20.0f;
  float icon_height = 18.0f;
  float spacing = 5.0f;
  std::string full_heart_tex = "assets/textures/UI/Heart.png";
  std::string empty_heart_tex = "assets/textures/UI/Heart-bg.png";

  auto health_panel = std::make_unique<engine::ui::UIPanel>();
  health_panel_ = health_panel.get();

  for (int i = 0; i < max_health; ++i) {
    glm::vec2 icon_pos = {start_x + i * (icon_width + spacing), start_y};
    glm::vec2 icon_size = {icon_width, icon_height};

    auto bg_icon = std::make_unique<engine::ui::UIImage>(empty_heart_tex,
                                                         icon_pos, icon_size);
    health_panel_->AddChild(std::move(bg_icon));
  }
  for (int i = 0; i < current_health; ++i) {
    glm::vec2 icon_pos = {start_x + i * (icon_width + spacing), start_y};
    glm::vec2 icon_size = {icon_width, icon_height};

    auto fg_icon = std::make_unique<engine::ui::UIImage>(full_heart_tex,
                                                         icon_pos, icon_size);
    health_panel_->AddChild(std::move(fg_icon));
  }
  ui_manager_->AddElement(std::move(health_panel));
}

void GameScene::UpdateHealthWithUI() {
  if (!player_ || !health_panel_) {
    GAME_LOG_ERROR("Player or health_panel_ is null, cannot update health UI.");
    return;
  }

  auto current_health =
      player_->GetComponent<engine::component::HealthComponent>()
          ->GetCurrentHealth();
  game_session_->SetCurrentHealth(current_health);
  auto max_health = game_session_->GetMaxHealth();

  for (auto i = max_health; i < max_health * 2; ++i) {
    health_panel_->GetChildren()[i]->SetVisible(i - max_health <
                                                current_health);
  }
}

void GameScene::AddScoreWithUI(int score) {
  game_session_->AddScore(score);
  auto score_text =
      "Score: " + std::to_string(game_session_->GetCurrentScore());
  score_label_->SetText(score_text);
}

void GameScene::HealWithUI(int amount) {
  player_->GetComponent<engine::component::HealthComponent>()->Heal(amount);
  UpdateHealthWithUI();
}

}  // namespace game::scene
