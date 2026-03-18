// Copyright Sylar129

#include "engine/core/context.h"

#include <fstream>

#include "SDL3/SDL.h"
#include "assert.h"
#include "engine/core/config.h"
#include "engine/core/context.h"
#include "engine/core/game_state.h"
#include "engine/core/time.h"
#include "engine/input/input_manager.h"
#include "engine/physics/physics_engine.h"
#include "engine/render/camera.h"
#include "engine/render/renderer.h"
#include "engine/render/text_renderer.h"
#include "engine/resource/audio_manager.h"
#include "engine/resource/resource_manager.h"
#include "engine/scene/scene_manager.h"
#include "utils/log.h"

namespace engine::core {

Context::Context() {}

Context::~Context() {}

bool Context::Init() {
  if (!InitConfig()) return false;

  if (!InitSDL()) return false;

  time_ = std::make_unique<Time>();
  time_->SetTargetFps(config_->performance.target_fps);

  resource_manager_ =
      std::make_unique<resource::ResourceManager>(sdl_renderer_);
  audio_manager_ = std::make_unique<resource::AudioManager>();

  renderer_ = std::make_unique<render::Renderer>(sdl_renderer_,
                                                 resource_manager_.get());

  camera_ = std::make_unique<render::Camera>(
      glm::vec2(config_->window.width / 2, config_->window.height / 2));

  text_renderer_ = std::make_unique<render::TextRenderer>(
      sdl_renderer_, resource_manager_.get());
  input_manager_ =
      std::make_unique<input::InputManager>(sdl_renderer_, config_.get());
  physics_engine_ = std::make_unique<physics::PhysicsEngine>();
  game_state_ = std::make_unique<core::GameState>(window_, sdl_renderer_);

  return true;
}

void Context::Clean() {
  resource_manager_.reset();
  audio_manager_.reset();

  if (sdl_renderer_ != nullptr) {
    SDL_DestroyRenderer(sdl_renderer_);
    sdl_renderer_ = nullptr;
  }
  if (window_ != nullptr) {
    SDL_DestroyWindow(window_);
    window_ = nullptr;
  }
  SDL_Quit();
}

bool Context::InitConfig() {
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

bool Context::InitSDL() {
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

  SDL_SetRenderDrawBlendMode(sdl_renderer_, SDL_BLENDMODE_BLEND);
  SDL_SetRenderLogicalPresentation(sdl_renderer_, config_->window.width / 2,
                                   config_->window.height / 2,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);

  return true;
}

}  // namespace engine::core
