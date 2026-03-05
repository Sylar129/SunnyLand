// Copyright Sylar129

#include "engine/core/game_app.h"

#include <memory>

#include "SDL3/SDL.h"
#include "engine/core/context.h"
#include "engine/core/time.h"
#include "engine/render/camera.h"
#include "engine/render/renderer.h"
#include "engine/resource/resource_manager.h"
#include "engine/scene/scene_manager.h"
#include "game/game_scene.h"
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

  while (is_running_) {
    time_->Update();
    float delta_time = time_->GetDeltaTime();
    input_manager_->Update();

    HandleEvents();
    Update(delta_time);
    Render();
  }

  Close();
}

bool GameApp::Init() {
  ENGINE_TRACE("Init GameApp ...");
  if (!InitConfig()) return false;
  if (!InitSDL()) return false;
  if (!InitTime()) return false;
  if (!InitResourceManager()) return false;
  if (!InitRenderer()) return false;
  if (!InitCamera()) return false;
  if (!InitInputManager()) return false;
  if (!InitContext()) return false;
  if (!InitSceneManager()) return false;

  auto scene = std::make_unique<game::scene::GameScene>("GameScene", *context_,
                                                        *scene_manager_);
  scene_manager_->RequestPushScene(std::move(scene));

  is_running_ = true;
  return true;
}

bool GameApp::InitConfig() {
  try {
    config_ = std::make_unique<engine::core::Config>("assets/config.json");
  } catch (const std::exception& e) {
    ENGINE_ERROR("Init Config failed: {}", e.what());
    return false;
  }
  ENGINE_TRACE("Init Config successfully.");
  return true;
}

bool GameApp::InitSDL() {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    ENGINE_ERROR("Failed to init SDL! Error: {}", SDL_GetError());
    return false;
  }

  window_ =
      SDL_CreateWindow(config_->window_title_.c_str(), config_->window_width_,
                       config_->window_height_, SDL_WINDOW_RESIZABLE);
  if (window_ == nullptr) {
    ENGINE_ERROR("Failed to create window! Error: {}", SDL_GetError());
    return false;
  }

  sdl_renderer_ = SDL_CreateRenderer(window_, nullptr);
  if (sdl_renderer_ == nullptr) {
    ENGINE_ERROR("Failed to create renderer! Error: {}", SDL_GetError());
    return false;
  }

  int vsync_mode = config_->vsync_enabled_ ? SDL_RENDERER_VSYNC_ADAPTIVE
                                           : SDL_RENDERER_VSYNC_DISABLED;
  SDL_SetRenderVSync(sdl_renderer_, vsync_mode);
  ENGINE_TRACE("Setting VSync: {}",
               config_->vsync_enabled_ ? "Enabled" : "Disabled");

  SDL_SetRenderLogicalPresentation(sdl_renderer_, config_->window_width_ / 2,
                                   config_->window_height_ / 2,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);

  return true;
}

bool GameApp::InitRenderer() {
  try {
    renderer_ = std::make_unique<engine::render::Renderer>(
        sdl_renderer_, resource_manager_.get());
  } catch (const std::exception& e) {
    ENGINE_ERROR("Renderer initialization failed: {}", e.what());
    return false;
  }
  ENGINE_TRACE("Renderer initialized successfully.");
  return true;
}

bool GameApp::InitCamera() {
  try {
    camera_ = std::make_unique<engine::render::Camera>(
        glm::vec2(config_->window_width_ / 2, config_->window_height_ / 2));
  } catch (const std::exception& e) {
    ENGINE_ERROR("Camera initialization failed: {}", e.what());
    return false;
  }
  ENGINE_TRACE("Camera initialized successfully.");
  return true;
}

bool GameApp::InitTime() {
  time_ = std::make_unique<Time>();
  time_->SetTargetFps(config_->target_fps_);

  return true;
}

bool GameApp::InitResourceManager() {
  resource_manager_ =
      std::make_unique<engine::resource::ResourceManager>(sdl_renderer_);
  return true;
}

bool GameApp::InitInputManager() {
  try {
    input_manager_ = std::make_unique<engine::input::InputManager>(
        sdl_renderer_, config_.get());
  } catch (const std::exception& e) {
    ENGINE_ERROR("Fail to init InputManager: {}", e.what());
    return false;
  }
  ENGINE_TRACE("Init InputManager successfully.");
  return true;
}

bool GameApp::InitContext() {
  try {
    context_ = std::make_unique<engine::core::Context>(
        *input_manager_, *renderer_, *camera_, *resource_manager_);
  } catch (const std::exception& e) {
    ENGINE_ERROR("Failed to init Context: {}", e.what());
    return false;
  }
  ENGINE_ERROR("Init Context successfully.");
  return true;
}

bool GameApp::InitSceneManager() {
  try {
    scene_manager_ = std::make_unique<engine::scene::SceneManager>(*context_);
  } catch (const std::exception& e) {
    ENGINE_ERROR("初始化场景管理器失败: {}", e.what());
    return false;
  }
  ENGINE_TRACE("场景管理器初始化成功。");
  return true;
}

void GameApp::HandleEvents() {
  if (input_manager_->ShouldQuit()) {
    ENGINE_TRACE("GameApp receive quit request from InputManager.");
    is_running_ = false;
    return;
  }
  scene_manager_->HandleInput();
}

void GameApp::Update(float delta_time) { scene_manager_->Update(delta_time); }

void GameApp::Render() {
  renderer_->ClearScreen();

  scene_manager_->Render();

  renderer_->Present();
}

void GameApp::Close() {
  ENGINE_TRACE("Closing GameApp...");
  resource_manager_.reset();

  if (sdl_renderer_ != nullptr) {
    SDL_DestroyRenderer(sdl_renderer_);
    sdl_renderer_ = nullptr;
  }
  if (window_ != nullptr) {
    SDL_DestroyWindow(window_);
    window_ = nullptr;
  }
  SDL_Quit();
  is_running_ = false;
}

}  // namespace engine::core
