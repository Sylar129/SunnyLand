// Copyright Sylar129

#include "game/scene/title_scene.h"

#include "engine/core/context.h"
#include "engine/core/game_state.h"
#include "engine/input/input_manager.h"
#include "engine/render/camera.h"
#include "engine/resource/resource_manager.h"
#include "engine/scene/level_loader.h"
#include "engine/scene/scene_manager.h"
#include "engine/ui/ui_button.h"
#include "engine/ui/ui_image.h"
#include "engine/ui/ui_label.h"
#include "engine/ui/ui_manager.h"
#include "engine/ui/ui_panel.h"
#include "engine/utils/math.h"
#include "game/data/session_data.h"
#include "game/scene/game_scene.h"
#include "game/scene/helps_scene.h"
#include "log.h"

namespace game::scene {

TitleScene::TitleScene(engine::core::Context& context,
                       engine::scene::SceneManager& scene_manager,
                       std::shared_ptr<game::data::Session> session_data)
    : engine::scene::Scene("TitleScene", context, scene_manager),
      session_data_(std::move(session_data)) {
  if (!session_data_) {
    GAME_WARN("TitleScene empty SessionData, creating default SessionData.");
    session_data_ = std::make_shared<game::data::Session>();
  }
  GAME_TRACE("TitleScene constructed");
}

void TitleScene::Init() {
  if (is_initialized_) {
    return;
  }
  engine::scene::LevelLoader level_loader;
  if (!level_loader.LoadLevel("assets/maps/level0.tmj", *this)) {
    GAME_ERROR("load title scene level failed!");
    return;
  }

  CreateUI();

  Scene::Init();
  GAME_TRACE("TitleScene initialized");
}

void TitleScene::CreateUI() {
  GAME_TRACE("Creating TitleScene UI...");
  context_.GetGameState().SetState(engine::core::State::kTitle);
  auto window_size = context_.GetGameState().GetLogicalSize();

  if (!ui_manager_->Init(window_size)) {
    GAME_ERROR("init UIManager failed!");
    return;
  }

  // context_.getAudioPlayer().setMusicVolume(0.2f);
  // context_.getAudioPlayer().setSoundVolume(0.5f);

  // context_.getAudioPlayer().playMusic("assets/audio/platformer_level03_loop.ogg");

  auto title_image = std::make_unique<engine::ui::UIImage>(
      "assets/textures/UI/title-screen.png");
  auto size =
      context_.GetResourceManager().GetTextureSize(title_image->GetTextureId());
  title_image->SetSize(size * 2.0f);

  auto title_pos =
      (window_size - title_image->GetSize()) / 2.0f - glm::vec2(0.0f, 50.0f);
  title_image->SetPosition(title_pos);
  ui_manager_->AddElement(std::move(title_image));

  float button_width = 96.0f;
  float button_height = 32.0f;
  float button_spacing = 20.0f;
  int num_buttons = 4;

  float panel_width =
      num_buttons * button_width + (num_buttons - 1) * button_spacing;
  float panel_height = button_height;

  float panel_x = (window_size.x - panel_width) / 2.0f;
  float panel_y = window_size.y * 0.65f;

  auto button_panel = std::make_unique<engine::ui::UIPanel>(
      glm::vec2(panel_x, panel_y), glm::vec2(panel_width, panel_height));

  glm::vec2 current_button_pos = glm::vec2(0.0f, 0.0f);
  glm::vec2 button_size = glm::vec2(button_width, button_height);

  auto start_button = std::make_unique<engine::ui::UIButton>(
      context_, "assets/textures/UI/buttons/Start1.png",
      "assets/textures/UI/buttons/Start2.png",
      "assets/textures/UI/buttons/Start3.png", current_button_pos, button_size,
      [this]() { this->OnStartGameClick(); });
  button_panel->AddChild(std::move(start_button));

  // Load Button
  current_button_pos.x += button_width + button_spacing;
  auto load_button = std::make_unique<engine::ui::UIButton>(
      context_, "assets/textures/UI/buttons/Load1.png",
      "assets/textures/UI/buttons/Load2.png",
      "assets/textures/UI/buttons/Load3.png", current_button_pos, button_size,
      [this]() { this->OnLoadGameClick(); });
  button_panel->AddChild(std::move(load_button));

  // Helps Button
  current_button_pos.x += button_width + button_spacing;
  auto helps_button = std::make_unique<engine::ui::UIButton>(
      context_, "assets/textures/UI/buttons/Helps1.png",
      "assets/textures/UI/buttons/Helps2.png",
      "assets/textures/UI/buttons/Helps3.png", current_button_pos, button_size,
      [this]() { this->OnHelpsClick(); });
  button_panel->AddChild(std::move(helps_button));

  // Quit Button
  current_button_pos.x += button_width + button_spacing;
  auto quit_button = std::make_unique<engine::ui::UIButton>(
      context_, "assets/textures/UI/buttons/Quit1.png",
      "assets/textures/UI/buttons/Quit2.png",
      "assets/textures/UI/buttons/Quit3.png", current_button_pos, button_size,
      [this]() { this->OnQuitClick(); });
  button_panel->AddChild(std::move(quit_button));

  ui_manager_->AddElement(std::move(button_panel));

  auto credits_label = std::make_unique<engine::ui::UILabel>(
      context_.GetTextRenderer(), "SunnyLand Credits: XXX - 2025",
      "assets/fonts/VonwaonBitmap-16px.ttf", 16,
      engine::utils::FColor{0.8f, 0.8f, 0.8f, 1.0f});
  credits_label->SetPosition(
      glm::vec2{(window_size.x - credits_label->GetSize().x) / 2.0f,
                window_size.y - credits_label->GetSize().y - 10.0f});
  ui_manager_->AddElement(std::move(credits_label));

  GAME_TRACE("TitleScene UI created successfully.");
}

void TitleScene::Update(float delta_time) {
  Scene::Update(delta_time);

  context_.GetCamera().Move(glm::vec2(delta_time * 100.0f, 0.0f));
}

void TitleScene::OnStartGameClick() {
  GAME_DEBUG("OnStartGameClick");
  scene_manager_.RequestReplaceScene(
      std::make_unique<GameScene>(context_, scene_manager_, session_data_));
}

void TitleScene::OnLoadGameClick() {
  GAME_DEBUG("OnLoadGameClick");
  if (!session_data_) {
    GAME_ERROR("SessionData is null, cannot load game.");
    return;
  }

  if (session_data_->LoadFromFile("assets/save.json")) {
    GAME_DEBUG(
        "Load game successfully from save.json, transitioning to GameScene.");
    scene_manager_.RequestReplaceScene(
        std::make_unique<GameScene>(context_, scene_manager_, session_data_));
  } else {
    GAME_WARN("Failed to load game from save.json. Starting new game instead.");
  }
}

void TitleScene::OnHelpsClick() {
  GAME_DEBUG("OnHelpsClick");
  scene_manager_.RequestPushScene(
      std::make_unique<HelpsScene>(context_, scene_manager_));
}

void TitleScene::OnQuitClick() {
  GAME_DEBUG("OnQuitClick");
  context_.GetInputManager().SetShouldQuit(true);
}

}  // namespace game::scene
