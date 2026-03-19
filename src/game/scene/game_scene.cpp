// Copyright Sylar129

#include "game/scene/game_scene.h"

#include "engine/core/context.h"
#include "engine/core/game_state.h"
#include "engine/ecs/components.h"
#include "engine/input/input_manager.h"
#include "engine/render/animation.h"
#include "engine/render/camera.h"
#include "engine/resource/resource_manager.h"
#include "engine/scene/ecs_level_loader.h"
#include "engine/scene/scene_manager.h"
#include "engine/ui/ui_image.h"
#include "engine/ui/ui_label.h"
#include "engine/ui/ui_manager.h"
#include "engine/ui/ui_panel.h"
#include "game/data/session_data.h"
#include "game/ecs/components.h"
#include "game/ecs/entity/effect_entity.h"
#include "game/ecs/entity/enemy_entity.h"
#include "game/ecs/entity/item_entity.h"
#include "game/ecs/entity/player_entity.h"
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
  engine::scene::Scene::Init();
  GAME_LOG_TRACE("Init GameScene '{}'", GetName());
}

void GameScene::Update(float delta_time) {
  if (!is_initialized_) {
    return;
  }

  player_system_.Update(registry_, delta_time);
  ai_system_.Update(registry_, delta_time);
  physics_system_.Update(registry_, delta_time);
  animation_system_.Update(registry_, delta_time);
  UpdateCamera();

  const auto events = game_rule_system_.Update(
      registry_, player_entity_, physics_system_.GetCollisionPairs(),
      physics_system_.GetTileTriggerEvents());
  ProcessRuleEvents(events);
  ui_manager_->Update(delta_time, context_);
  engine::ecs::DestroyPendingEntities(registry_);

  if (registry_.valid(player_entity_) &&
      registry_.all_of<engine::ecs::TransformComponent>(player_entity_)) {
    const auto& transform =
        registry_.get<engine::ecs::TransformComponent>(player_entity_);
    const auto& world_rect = physics_system_.GetWorldBounds();
    if (world_rect && transform.position.y > world_rect->position.y +
                                                 world_rect->size.y + 100.0f) {
      ShowEndScene(false);
    }
  }
}

void GameScene::Render() {
  if (!is_initialized_) {
    return;
  }
  render_system_.Render(registry_, context_);
  ui_manager_->Render(context_);
}

void GameScene::HandleInput() {
  if (!is_initialized_) {
    return;
  }
  if (ui_manager_->HandleInput(context_)) {
    return;
  }
  player_system_.HandleInput(registry_, context_.GetInputManager());
  if (context_.GetInputManager().IsActionPressed("pause")) {
    scene_manager_.RequestPushScene(
        std::make_unique<MenuScene>(context_, scene_manager_, game_session_));
  }
}

void GameScene::Clean() {
  player_entity_ = entt::null;
  engine::scene::Scene::Clean();
}

void GameScene::InitLevel() {
  engine::scene::EcsLevelLoader level_loader;
  auto level_path = game_session_->GetCurrentMapPath();
  if (!level_loader.LoadLevel(level_path, context_, registry_)) {
    GAME_LOG_ERROR("Load level '{}' failed!", level_path);
    return;
  }

  const auto main_entity = engine::scene::Scene::FindEntityByName("main");
  if (main_entity == entt::null ||
      !registry_.all_of<engine::ecs::TileLayerComponent>(main_entity)) {
    GAME_LOG_ERROR("No main tile layer found in level '{}'.", level_path);
    return;
  }

  const auto& main_layer =
      registry_.get<engine::ecs::TileLayerComponent>(main_entity);
  const auto world_size =
      glm::vec2(main_layer.map_size.x * main_layer.tile_size.x,
                main_layer.map_size.y * main_layer.tile_size.y);
  context_.GetCamera().SetLimitBounds(utils::Rect(glm::vec2(0.0f), world_size));
  context_.GetCamera().SetPosition(glm::vec2(0.0f));
  physics_system_.SetWorldBounds(utils::Rect(glm::vec2(0.0f), world_size));
}

void GameScene::InitPlayer() {
  player_entity_ = engine::scene::Scene::FindEntityByName("player");
  GAME_LOG_ASSERT(player_entity_ != entt::null, "No Player!");
  game::ecs::entity::ConfigurePlayerEntity(registry_, player_entity_,
                                           game_session_->GetCurrentHealth(),
                                           game_session_->GetMaxHealth());
}

void GameScene::InitEnemyAndItem() {
  auto view =
      registry_
          .view<engine::ecs::NameComponent, engine::ecs::TransformComponent>();
  for (const auto entity : view) {
    game::ecs::entity::ConfigureEnemyEntity(registry_, entity);
    game::ecs::entity::ConfigureItemEntity(registry_, entity);
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

void GameScene::UpdateCamera() {
  if (!registry_.valid(player_entity_) ||
      !registry_.all_of<engine::ecs::TransformComponent>(player_entity_)) {
    return;
  }
  const auto& transform =
      registry_.get<engine::ecs::TransformComponent>(player_entity_);
  const auto desired_position =
      transform.position - context_.GetCamera().GetViewportSize() / 2.0f;
  context_.GetCamera().SetPosition(desired_position);
}

void GameScene::ProcessRuleEvents(const game::ecs::RuleEvents& events) {
  if (events.player_damage > 0) {
    HandlePlayerDamage(events.player_damage);
  }
  if (events.heal_amount > 0) {
    HealWithUI(events.heal_amount);
  }
  if (events.score_delta > 0) {
    AddScoreWithUI(events.score_delta);
  }
  for (const auto entity : events.destroy_entities) {
    if (registry_.valid(entity)) {
      registry_.emplace_or_replace<engine::ecs::PendingDestroyComponent>(
          entity);
    }
  }
  for (const auto& effect : events.effects) {
    CreateEffect(effect.center, effect.kind);
  }
  if (!events.next_level_name.empty()) {
    ToNextLevel(events.next_level_name);
  }
  if (events.request_win) {
    ShowEndScene(true);
  }
}

void GameScene::HandlePlayerDamage(int damage) {
  if (!player_system_.ApplyDamage(registry_, player_entity_, damage)) {
    return;
  }
  const auto& player =
      registry_.get<game::ecs::PlayerComponent>(player_entity_);
  if (player.is_dead) {
    GAME_LOG_INFO("Player is dead");
  }
  UpdateHealthWithUI();
}

void GameScene::CreateEffect(const glm::vec2& center_pos,
                             const std::string& tag) {
  const auto effect_entity = game::ecs::entity::CreateEffectEntity(
      registry_, context_.GetResourceManager(), center_pos, tag);
  if (effect_entity == entt::null) {
    GAME_LOG_WARN("Unknown effect type: {}", tag);
    return;
  }
  GAME_LOG_DEBUG("Created effect: {}", tag);
}

void GameScene::ToNextLevel(const std::string& level_name) {
  auto map_path = LevelNameToPath(level_name);
  game_session_->SetNextMapPath(map_path);
  std::unique_ptr<engine::scene::Scene> next_scene =
      std::make_unique<game::scene::GameScene>(context_, scene_manager_,
                                               game_session_);
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
    glm::vec2 icon_pos = {
        start_x + static_cast<float>(i) * (icon_width + spacing), start_y};
    glm::vec2 icon_size = {icon_width, icon_height};

    auto bg_icon = std::make_unique<engine::ui::UIImage>(empty_heart_tex,
                                                         icon_pos, icon_size);
    health_panel_->AddChild(std::move(bg_icon));
  }
  for (int i = 0; i < current_health; ++i) {
    glm::vec2 icon_pos = {
        start_x + static_cast<float>(i) * (icon_width + spacing), start_y};
    glm::vec2 icon_size = {icon_width, icon_height};

    auto fg_icon = std::make_unique<engine::ui::UIImage>(full_heart_tex,
                                                         icon_pos, icon_size);
    health_panel_->AddChild(std::move(fg_icon));
  }
  ui_manager_->AddElement(std::move(health_panel));
}

void GameScene::UpdateHealthWithUI() {
  if (!registry_.valid(player_entity_) || !health_panel_) {
    GAME_LOG_ERROR("Player or health_panel_ is null, cannot update health UI.");
    return;
  }

  const auto current_health =
      registry_.get<engine::ecs::HealthComponent>(player_entity_)
          .current_health;
  game_session_->SetCurrentHealth(current_health);
  auto max_health = game_session_->GetMaxHealth();

  for (auto i = max_health; i < max_health * 2; ++i) {
    health_panel_->GetChildren()[static_cast<size_t>(i)]->SetVisible(
        i - max_health < current_health);
  }
}

void GameScene::AddScoreWithUI(int score) {
  game_session_->AddScore(score);
  auto score_text =
      "Score: " + std::to_string(game_session_->GetCurrentScore());
  score_label_->SetText(score_text);
}

void GameScene::HealWithUI(int amount) {
  if (!registry_.valid(player_entity_) ||
      !registry_.all_of<engine::ecs::HealthComponent>(player_entity_)) {
    return;
  }
  auto& health = registry_.get<engine::ecs::HealthComponent>(player_entity_);
  health.current_health =
      std::min(health.max_health, health.current_health + amount);
  UpdateHealthWithUI();
}

}  // namespace game::scene
