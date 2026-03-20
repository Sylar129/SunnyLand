// Copyright Sylar129

#include "engine/core/game_app.h"

#include <memory>

#include "engine/core/context.h"
#include "engine/core/time.h"
#include "engine/input/input_manager.h"
#include "engine/render/renderer.h"
#include "engine/scene/scene_manager.h"
#include "utils/assert.h"
#include "utils/log.h"

namespace engine::core {

GameApp::GameApp() {}

GameApp::~GameApp() {
  if (is_running_) {
    ENGINE_LOG_WARN("GameApp is still running when dtor!");
    Close();
  }
}

void GameApp::Run() {
  while (is_running_) {
    context_->GetTime().Update();
    float delta_time = context_->GetTime().GetDeltaTime();
    context_->GetInputManager().Update();

    HandleEvents();
    Update(delta_time);
    Render();
  }
}

bool GameApp::Init() {
  ENGINE_LOG_TRACE("Init GameApp ...");
  if (!InitContext()) return false;

  if (!InitSceneManager()) return false;

  is_running_ = true;
  return true;
}

bool GameApp::InitContext() {
  context_ = std::make_unique<core::Context>();
  bool initialized = context_->Init();
  if (!initialized) {
    ENGINE_LOG_ERROR("Failed to init Context!");
    return false;
  }

  ENGINE_LOG_TRACE("Init Context successfully.");
  return true;
}

bool GameApp::InitSceneManager() {
  scene_manager_ = std::make_unique<scene::SceneManager>(*context_);
  ENGINE_LOG_ASSERT(scene_manager_, "Failed to Init SceneManager!");

  ENGINE_LOG_TRACE("Init SceneManager successfully.");
  return true;
}

void GameApp::HandleEvents() {
  if (context_->GetInputManager().ShouldQuit()) {
    ENGINE_LOG_TRACE("GameApp receive quit request from InputManager.");
    is_running_ = false;
    return;
  }
  scene_manager_->HandleInput();
}

void GameApp::Update(float delta_time) { scene_manager_->Update(delta_time); }

void GameApp::Render() {
  context_->GetRenderer().ClearScreen();

  scene_manager_->Render();

  context_->GetRenderer().Present();
}

void GameApp::Close() {
  ENGINE_LOG_TRACE("Closing GameApp...");
  // Stop the run loop first so no further updates occur during shutdown.
  is_running_ = false;
  // Close scenes before cleaning the context, since scenes may rely on it.
  if (scene_manager_) {
    scene_manager_->Close();
  }
  // Clean the context after scenes are closed; safe even if init failed.
  if (context_) {
    context_->Clean();
  }
}

}  // namespace engine::core
