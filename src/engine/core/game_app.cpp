// Copyright Sylar129

#include "engine/core/game_app.h"

#include <memory>

#include "SDL3/SDL.h"
#include "engine/core/time.h"
#include "engine/resource/resource_manager.h"
#include "spdlog/spdlog.h"

namespace engine::core {

GameApp::GameApp() : time_(std::make_unique<Time>()) {}

GameApp::~GameApp() {
  if (is_running_) {
    SPDLOG_WARN("GameApp is still running when dtor!");
    Close();
  }
}

void GameApp::Run() {
  if (!Init()) {
    SPDLOG_ERROR("Failed to init game!");
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
  spdlog::trace("Init GameApp ...");
  if (!initSDL()) return false;
  if (!initTime()) return false;
  if (!initResourceManager()) return false;

  is_running_ = true;
  return true;
}

bool GameApp::initSDL() {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    SPDLOG_ERROR("Failed to init SDL! Error: {}", SDL_GetError());
    return false;
  }

  window_ = SDL_CreateWindow("SunnyLand", 1280, 720, SDL_WINDOW_RESIZABLE);
  if (window_ == nullptr) {
    SPDLOG_ERROR("Failed to create window! Error: {}", SDL_GetError());
    return false;
  }

  renderer_ = SDL_CreateRenderer(window_, nullptr);
  if (renderer_ == nullptr) {
    SPDLOG_ERROR("Failed to create renderer! Error: {}", SDL_GetError());
    return false;
  }
  return true;
}

bool GameApp::initTime() {
  time_ = std::make_unique<Time>();
  return true;
}

bool GameApp::initResourceManager() {
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

void GameApp::Update(float delta_time) {
}

void GameApp::Render() {
}

void GameApp::Close() {
  SPDLOG_TRACE("Closing GameApp...");
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
