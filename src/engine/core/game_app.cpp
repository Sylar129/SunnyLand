// Copyright Sylar129

#include "engine/core/game_app.h"

#include <fstream>
#include <memory>

#include "SDL3/SDL.h"
#include "assert.h"
#include "engine/core/config.h"
#include "engine/core/context.h"
#include "engine/core/game_state.h"
#include "engine/core/time.h"
#include "engine/physics/physics_engine.h"
#include "engine/render/camera.h"
#include "engine/render/renderer.h"
#include "engine/resource/resource_manager.h"
#include "engine/scene/scene_manager.h"
#include "game/scene/title_scene.h"
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
  if (!Init()) {
    ENGINE_LOG_ERROR("Failed to init game!");
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
  ENGINE_LOG_TRACE("Init GameApp ...");
  if (!InitConfig()) return false;

  if (!InitSDL()) return false;
  if (!InitTime()) return false;
  if (!InitResourceManager()) return false;
  if (!InitAudioManager()) return false;
  if (!InitRenderer()) return false;
  if (!InitCamera()) return false;
  if (!InitTextRenderer()) return false;
  if (!InitInputManager()) return false;
  if (!InitPhysicsEngine()) return false;
  if (!InitGameState()) return false;
  if (!InitContext()) return false;

  if (!InitSceneManager()) return false;

  is_running_ = true;
  return true;
}

bool GameApp::InitConfig() {
  constexpr const char* filepath = "assets/config.json";
  std::ifstream file(filepath);
  if (!file.is_open()) {
    ENGINE_LOG_WARN(
        "Config file '{}' not found. Using default settings and creating "
        "default config file.",
        filepath);

    std::ofstream output_file(filepath);
    if (output_file.is_open()) {
      output_file << nlohmann::json(Config()).dump(4);
    } else {
      ENGINE_LOG_ERROR("Failed to create default config file at '{}'",
                       filepath);
    }

    config_ = std::make_unique<core::Config>();
    ENGINE_LOG_TRACE("Init Config successfully.");
    return true;
  }

  try {
    nlohmann::json j = nlohmann::json::parse(file);
    config_ = std::make_unique<core::Config>(j.get<Config>());
  } catch (const std::exception& e) {
    ENGINE_LOG_ERROR(
        "Error reading config file '{}': {}. Using default settings.", filepath,
        e.what());
    config_ = std::make_unique<core::Config>();
  }

  ENGINE_LOG_TRACE("Init Config successfully.");
  return true;
}

bool GameApp::InitSDL() {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    ENGINE_LOG_ERROR("Failed to init SDL! Error: {}", SDL_GetError());
    return false;
  }

  window_ =
      SDL_CreateWindow(config_->window.title.c_str(), config_->window.width,
                       config_->window.height,
                       config_->window.resizable ? SDL_WINDOW_RESIZABLE : 0);
  if (window_ == nullptr) {
    ENGINE_LOG_ERROR("Failed to create window! Error: {}", SDL_GetError());
    return false;
  }

  sdl_renderer_ = SDL_CreateRenderer(window_, nullptr);
  if (sdl_renderer_ == nullptr) {
    ENGINE_LOG_ERROR("Failed to create renderer! Error: {}", SDL_GetError());
    return false;
  }

  int vsync_mode = config_->graphics.vsync ? SDL_RENDERER_VSYNC_ADAPTIVE
                                           : SDL_RENDERER_VSYNC_DISABLED;
  SDL_SetRenderVSync(sdl_renderer_, vsync_mode);
  ENGINE_LOG_TRACE("Setting VSync: {}",
                   config_->graphics.vsync ? "Enabled" : "Disabled");

  SDL_SetRenderLogicalPresentation(sdl_renderer_, config_->window.width / 2,
                                   config_->window.height / 2,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);

  return true;
}

bool GameApp::InitRenderer() {
  renderer_ = std::make_unique<render::Renderer>(sdl_renderer_,
                                                 resource_manager_.get());

  SDL_SetRenderDrawBlendMode(sdl_renderer_, SDL_BLENDMODE_BLEND);

  ENGINE_LOG_ASSERT(renderer_, "Failed to Init Renderer!");

  ENGINE_LOG_TRACE("Renderer initialized successfully.");
  return true;
}

bool GameApp::InitCamera() {
  camera_ = std::make_unique<render::Camera>(
      glm::vec2(config_->window.width / 2, config_->window.height / 2));
  ENGINE_LOG_ASSERT(camera_, "Failed to Init Camera!");

  ENGINE_LOG_TRACE("Camera initialized successfully.");
  return true;
}

bool GameApp::InitTextRenderer() {
  text_renderer_ = std::make_unique<render::TextRenderer>(
      sdl_renderer_, resource_manager_.get());
  ENGINE_LOG_TRACE("TextRenderer initialized successfully.");
  return true;
}

bool GameApp::InitTime() {
  time_ = std::make_unique<Time>();
  time_->SetTargetFps(config_->performance.target_fps);

  return true;
}

bool GameApp::InitResourceManager() {
  resource_manager_ =
      std::make_unique<resource::ResourceManager>(sdl_renderer_);
  return true;
}

bool GameApp::InitAudioManager() {
  audio_manager_ = std::make_unique<resource::AudioManager>();
  return true;
}

bool GameApp::InitInputManager() {
  input_manager_ =
      std::make_unique<input::InputManager>(sdl_renderer_, config_.get());
  ENGINE_LOG_ASSERT(input_manager_, "Failed to Init InputManager!");

  ENGINE_LOG_TRACE("Init InputManager successfully.");
  return true;
}

bool GameApp::InitGameState() {
  game_state_ = std::make_unique<core::GameState>(window_, sdl_renderer_);

  return true;
}

bool GameApp::InitContext() {
  context_ = std::make_unique<core::Context>(
      *input_manager_, *renderer_, *camera_, *text_renderer_,
      *resource_manager_, *audio_manager_, *physics_engine_, *game_state_);
  ENGINE_LOG_ASSERT(context_, "Failed to Init Context!");

  ENGINE_LOG_TRACE("Init Context successfully.");
  return true;
}

bool GameApp::InitSceneManager() {
  scene_manager_ = std::make_unique<scene::SceneManager>(*context_);
  ENGINE_LOG_ASSERT(scene_manager_, "Failed to Init SceneManager!");

  auto scene =
      std::make_unique<game::scene::TitleScene>(*context_, *scene_manager_);
  scene_manager_->RequestPushScene(std::move(scene));

  ENGINE_LOG_TRACE("Init SceneManager successfully.");
  return true;
}

bool GameApp::InitPhysicsEngine() {
  physics_engine_ = std::make_unique<physics::PhysicsEngine>();
  return true;
}

void GameApp::HandleEvents() {
  if (input_manager_->ShouldQuit()) {
    ENGINE_LOG_TRACE("GameApp receive quit request from InputManager.");
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
  ENGINE_LOG_TRACE("Closing GameApp...");
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
