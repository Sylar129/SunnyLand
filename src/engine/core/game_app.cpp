// Copyright Sylar129

#include "engine/core/game_app.h"

#include <memory>

#include "SDL3/SDL.h"
#include "engine/core/time.h"
#include "engine/resource/resource_manager.h"
#include "log.h"

namespace engine::core {

GameApp::GameApp() {}

GameApp::~GameApp() {
  if (is_running_) {
    ENGINE_WARN("GameApp is still running when dtor!");
    Close();
  }
}

void GameApp::Run() {
  if (!Init()) {
    ENGINE_ERROR("Failed to init game!");
    return;
  }
  time_->SetTargetFps(144);

  while (is_running_) {
    time_->Update();
    float delta_time = time_->GetDeltaTime();

    HandleEvents();
    Update(delta_time);
    Render();
  }

  Close();
}

bool GameApp::Init() {
  ENGINE_TRACE("Init GameApp ...");
  if (!InitSDL()) return false;
  if (!InitTime()) return false;
  if (!InitResourceManager()) return false;

  is_running_ = true;
  return true;
}

bool GameApp::InitSDL() {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    ENGINE_ERROR("Failed to init SDL! Error: {}", SDL_GetError());
    return false;
  }

  window_ = SDL_CreateWindow("SunnyLand", 1280, 720, SDL_WINDOW_RESIZABLE);
  if (window_ == nullptr) {
    ENGINE_ERROR("Failed to create window! Error: {}", SDL_GetError());
    return false;
  }

  renderer_ = SDL_CreateRenderer(window_, nullptr);
  if (renderer_ == nullptr) {
    ENGINE_ERROR("Failed to create renderer! Error: {}", SDL_GetError());
    return false;
  }
  return true;
}

bool GameApp::InitTime() {
  time_ = std::make_unique<Time>();
  return true;
}

bool GameApp::InitResourceManager() {
  resource_manager_ =
      std::make_unique<engine::resource::ResourceManager>(renderer_);
  return true;
}

void GameApp::HandleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      is_running_ = false;
    }
  }
}

void GameApp::Update(float delta_time) {}

void GameApp::Render() {}

void GameApp::Close() {
  ENGINE_TRACE("Closing GameApp...");
  resource_manager_.reset();

  if (renderer_ != nullptr) {
    SDL_DestroyRenderer(renderer_);
    renderer_ = nullptr;
  }
  if (window_ != nullptr) {
    SDL_DestroyWindow(window_);
    window_ = nullptr;
  }
  SDL_Quit();
  is_running_ = false;
}

}  // namespace engine::core
