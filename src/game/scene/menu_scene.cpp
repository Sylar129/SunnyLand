// Copyright Sylar129

#include "game/scene/menu_scene.h"

#include "engine/core/context.h"
#include "engine/core/game_state.h"
#include "engine/input/input_manager.h"
#include "engine/scene/scene_manager.h"
#include "engine/ui/ui_button.h"
#include "engine/ui/ui_label.h"
#include "engine/ui/ui_manager.h"
#include "game/data/session_data.h"
#include "game/scene/title_scene.h"
#include "log.h"

namespace game::scene {

MenuScene::MenuScene(engine::core::Context& context,
                     engine::scene::SceneManager& scene_manager,
                     std::shared_ptr<game::data::Session> session_data)
    : Scene("MenuScene", context, scene_manager),
      session_data_(std::move(session_data)) {
  if (!session_data_) {
    GAME_ERROR("MenuScene session_data is null, creating default SessionData.");
  }
  GAME_TRACE("MenuScene constructed");
}

void MenuScene::Init() {
  context_.GetGameState().SetState(engine::core::State::Paused);
  CreateUI();

  Scene::Init();
  GAME_TRACE("MenuScene initialized");
}

void MenuScene::CreateUI() {
  auto window_size = context_.GetGameState().GetLogicalSize();
  if (!ui_manager_->Init(window_size)) {
    GAME_ERROR("MenuScene Failed to initialize UIManager!");
    return;
  }

  auto pause_label = std::make_unique<engine::ui::UILabel>(
      context_.GetTextRenderer(), "PAUSE",
      "assets/fonts/VonwaonBitmap-16px.ttf", 32);

  auto size = pause_label->GetSize();
  auto label_y = window_size.y * 0.2;
  pause_label->SetPosition(glm::vec2((window_size.x - size.x) / 2.0f, label_y));
  ui_manager_->AddElement(std::move(pause_label));

  float button_width = 96.0f;
  float button_height = 32.0f;
  float button_spacing = 10.0f;
  float start_y = label_y + 80.0f;
  float button_x = (window_size.x - button_width) / 2.0f;

  // Resume Button
  auto resume_button = std::make_unique<engine::ui::UIButton>(
      context_, "assets/textures/UI/buttons/Resume1.png",
      "assets/textures/UI/buttons/Resume2.png",
      "assets/textures/UI/buttons/Resume3.png", glm::vec2{button_x, start_y},
      glm::vec2{button_width, button_height},
      [this]() { this->OnResumeClicked(); });
  ui_manager_->AddElement(std::move(resume_button));

  // Save Button
  start_y += button_height + button_spacing;
  auto save_button = std::make_unique<engine::ui::UIButton>(
      context_, "assets/textures/UI/buttons/Save1.png",
      "assets/textures/UI/buttons/Save2.png",
      "assets/textures/UI/buttons/Save3.png", glm::vec2{button_x, start_y},
      glm::vec2{button_width, button_height},
      [this]() { this->OnSaveClicked(); });
  ui_manager_->AddElement(std::move(save_button));

  // Back Button
  start_y += button_height + button_spacing;
  auto back_button = std::make_unique<engine::ui::UIButton>(
      context_, "assets/textures/UI/buttons/Back1.png",
      "assets/textures/UI/buttons/Back2.png",
      "assets/textures/UI/buttons/Back3.png", glm::vec2{button_x, start_y},
      glm::vec2{button_width, button_height},
      [this]() { this->OnBackClicked(); });
  ui_manager_->AddElement(std::move(back_button));

  // Quit Button
  start_y += button_height + button_spacing;
  auto quit_button = std::make_unique<engine::ui::UIButton>(
      context_, "assets/textures/UI/buttons/Quit1.png",
      "assets/textures/UI/buttons/Quit2.png",
      "assets/textures/UI/buttons/Quit3.png", glm::vec2{button_x, start_y},
      glm::vec2{button_width, button_height},
      [this]() { this->OnQuitClicked(); });
  ui_manager_->AddElement(std::move(quit_button));
}

void MenuScene::HandleInput() {
  Scene::HandleInput();

  if (context_.GetInputManager().IsActionPressed("pause")) {
    GAME_DEBUG("Pause action detected in MenuScene, resuming game.");
    scene_manager_.RequestPopScene();
    context_.GetGameState().SetState(engine::core::State::Playing);
  }
}

void MenuScene::OnResumeClicked() {
  GAME_DEBUG("OnResumeClicked");
  scene_manager_.RequestPopScene();
  context_.GetGameState().SetState(engine::core::State::Playing);
}

void MenuScene::OnSaveClicked() {
  GAME_DEBUG("OnSaveClicked");
  if (session_data_ && session_data_->SaveToFile("assets/save.json")) {
    GAME_DEBUG("Save game successfully to save.json.");
  } else {
    GAME_ERROR(
        "Failed to save game. SessionData is null or failed to write to file.");
  }
}

void MenuScene::OnBackClicked() {
  GAME_DEBUG("OnBackClicked");
  scene_manager_.RequestReplaceScene(
      std::make_unique<TitleScene>(context_, scene_manager_, session_data_));
}

void MenuScene::OnQuitClicked() {
  GAME_DEBUG("OnQuitClicked");
  context_.GetInputManager().SetShouldQuit(true);
}

}  // namespace game::scene
