// Copyright Sylar129

#include "game/scene/end_scene.h"

#include "engine/core/context.h"
#include "engine/core/game_state.h"
#include "engine/scene/scene_manager.h"
#include "engine/ui/ui_button.h"
#include "engine/ui/ui_label.h"
#include "engine/ui/ui_manager.h"
#include "game/data/session_data.h"
#include "game/scene/game_scene.h"
#include "game/scene/title_scene.h"
#include "utils/assert.h"
#include "utils/log.h"
#include "utils/math.h"

namespace game::scene {

EndScene::EndScene(engine::core::Context& context,
                   engine::scene::SceneManager& scene_manager,
                   std::shared_ptr<game::data::Session> session_data)
    : engine::scene::Scene("EndScene", context, scene_manager),
      session_data_(std::move(session_data)) {
  GAME_LOG_ASSERT(session_data_,
                  "EndScene requires a valid SessionData pointer!");
  GAME_LOG_TRACE("EndScene constructed, is_win: {}", session_data_->GetIsWin());
}

void EndScene::Init() {
  if (is_initialized_) {
    return;
  }

  context_.GetGameState().SetState(engine::core::State::kGameOver);

  CreateUI();

  Scene::Init();
  GAME_LOG_INFO("EndScene initialized");
}

void EndScene::CreateUI() {
  auto window_size = context_.GetGameState().GetLogicalSize();
  if (!ui_manager_->Init(window_size)) {
    GAME_LOG_ERROR("EndScene failed to initialize UIManager!");
    return;
  }
  auto is_win = session_data_->GetIsWin();

  std::string main_message =
      is_win ? "YOU WIN! CONGRATS!" : "YOU DIED! TRY AGAIN!";
  engine::utils::FColor message_color =
      is_win ? engine::utils::FColor{0.0f, 1.0f, 0.0f, 1.0f}
             : engine::utils::FColor{1.0f, 0.0f, 0.0f, 1.0f};

  auto main_label = std::make_unique<engine::ui::UILabel>(
      context_.GetTextRenderer(), main_message,
      "assets/fonts/VonwaonBitmap-16px.ttf", 48, message_color);

  glm::vec2 label_size = main_label->GetSize();
  glm::vec2 main_label_pos = {(window_size.x - label_size.x) / 2.0f,
                              window_size.y * 0.3f};
  main_label->SetPosition(main_label_pos);
  ui_manager_->AddElement(std::move(main_label));

  int current_score = session_data_->GetCurrentScore();
  int high_score = session_data_->GetHighScore();
  engine::utils::FColor score_color = {1.0f, 1.0f, 1.0f, 1.0f};
  int score_font_size = 24;

  std::string score_text = "Score: " + std::to_string(current_score);
  auto score_label = std::make_unique<engine::ui::UILabel>(
      context_.GetTextRenderer(), score_text,
      "assets/fonts/VonwaonBitmap-16px.ttf", score_font_size, score_color);
  glm::vec2 score_label_size = score_label->GetSize();

  glm::vec2 score_label_pos = {(window_size.x - score_label_size.x) / 2.0f,
                               main_label_pos.y + label_size.y + 20.0f};
  score_label->SetPosition(score_label_pos);
  ui_manager_->AddElement(std::move(score_label));

  std::string high_score_text = "High Score: " + std::to_string(high_score);
  auto high_score_label = std::make_unique<engine::ui::UILabel>(
      context_.GetTextRenderer(), high_score_text,
      "assets/fonts/VonwaonBitmap-16px.ttf", score_font_size, score_color);
  glm::vec2 high_score_label_size = high_score_label->GetSize();

  glm::vec2 high_score_label_pos = {
      (window_size.x - high_score_label_size.x) / 2.0f,
      score_label_pos.y + score_label_size.y + 10.0f};
  high_score_label->SetPosition(high_score_label_pos);
  ui_manager_->AddElement(std::move(high_score_label));

  glm::vec2 button_size = {120.0f, 40.0f};
  float button_spacing = 20.0f;
  float total_button_width = button_size.x * 2 + button_spacing;

  float buttons_x = window_size.x - total_button_width - 30.0f;
  float buttons_y = window_size.y - button_size.y - 30.0f;

  // Back Button
  auto back_button = std::make_unique<engine::ui::UIButton>(
      context_, "assets/textures/UI/buttons/Back1.png",
      "assets/textures/UI/buttons/Back2.png",
      "assets/textures/UI/buttons/Back3.png", glm::vec2{buttons_x, buttons_y},
      button_size, [this]() { this->OnBackClick(); });
  ui_manager_->AddElement(std::move(back_button));

  // Restart Button
  buttons_x += button_size.x + button_spacing;
  auto restart_button = std::make_unique<engine::ui::UIButton>(
      context_, "assets/textures/UI/buttons/Restart1.png",
      "assets/textures/UI/buttons/Restart2.png",
      "assets/textures/UI/buttons/Restart3.png",
      glm::vec2{buttons_x, buttons_y}, button_size,
      [this]() { this->OnRestartClick(); });
  ui_manager_->AddElement(std::move(restart_button));
}

void EndScene::OnBackClick() {
  GAME_LOG_INFO("onBackClick");
  scene_manager_.RequestReplaceScene(
      std::make_unique<TitleScene>(context_, scene_manager_, session_data_));
}

void EndScene::OnRestartClick() {
  GAME_LOG_INFO("onRestartClick");
  session_data_ = nullptr;
  scene_manager_.RequestReplaceScene(
      std::make_unique<GameScene>(context_, scene_manager_, session_data_));
}

}  // namespace game::scene
