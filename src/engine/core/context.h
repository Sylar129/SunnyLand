// Copyright Sylar129

#pragma once

#include "engine/core/config.h"
#include "engine/core/time.h"
#include "engine/render/text_renderer.h"
#include "engine/resource/audio_manager.h"
#include "utils/non_copyable.h"

namespace engine::input {
class InputManager;
}

namespace engine::render {
class Renderer;
class Camera;
class TextRenderer;
}  // namespace engine::render

namespace engine::resource {
class ResourceManager;
class AudioManager;
}  // namespace engine::resource

namespace engine::physics {
class PhysicsEngine;
}

namespace engine::core {

class GameState;
struct Config;
class Time;

class Context final {
 public:
  Context();
  ~Context();
  DISABLE_COPY_AND_MOVE(Context);

  bool Init();
  void Clean();

  input::InputManager& GetInputManager() const { return *input_manager_; }
  render::Renderer& GetRenderer() const { return *renderer_; }
  render::Camera& GetCamera() const { return *camera_; }
  render::TextRenderer& GetTextRenderer() const { return *text_renderer_; }
  resource::ResourceManager& GetResourceManager() const {
    return *resource_manager_;
  }
  core::Time& GetTime() const { return *time_; }
  resource::AudioManager& GetAudioManager() const { return *audio_manager_; }
  physics::PhysicsEngine& GetPhysicsEngine() const { return *physics_engine_; }
  core::GameState& GetGameState() const { return *game_state_; }

 private:
  bool InitConfig();

  bool InitSDL();

  std::unique_ptr<core::Config> config_;
  SDL_Window* window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;

  std::unique_ptr<Time> time_;
  std::unique_ptr<resource::ResourceManager> resource_manager_;
  std::unique_ptr<resource::AudioManager> audio_manager_;
  std::unique_ptr<render::Renderer> renderer_;
  std::unique_ptr<render::Camera> camera_;
  std::unique_ptr<render::TextRenderer> text_renderer_;
  std::unique_ptr<input::InputManager> input_manager_;
  std::unique_ptr<physics::PhysicsEngine> physics_engine_;
  std::unique_ptr<core::GameState> game_state_;
};

}  // namespace engine::core
