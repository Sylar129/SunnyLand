// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/core/config.h"
#include "engine/core/time.h"
#include "engine/input/input_manager.h"
#include "engine/render/text_renderer.h"
#include "engine/resource/audio_manager.h"
#include "utils/non_copyable.h"

struct SDL_Window;
struct SDL_Renderer;

namespace engine::render {
class Renderer;
class Camera;
class TextRenderer;
}  // namespace engine::render

namespace engine::resource {
class ResourceManager;
}

namespace engine::scene {
class SceneManager;
}

namespace engine::physics {
class PhysicsEngine;
}

namespace engine::core {

class GameState;
class Context;

class GameApp final {
 public:
  GameApp();
  ~GameApp();
  DISABLE_COPY_AND_MOVE(GameApp);

  void Run();

 private:
  bool Init();
  bool InitConfig();
  bool InitSDL();
  bool InitRenderer();
  bool InitCamera();
  bool InitTextRenderer();
  bool InitTime();
  bool InitResourceManager();
  bool InitAudioManager();
  bool InitInputManager();
  bool InitGameState();
  bool InitContext();
  bool InitSceneManager();
  bool InitPhysicsEngine();

  void HandleEvents();
  void Update(float delta_time);
  void Render();
  void Close();

  SDL_Window* window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;
  bool is_running_ = false;
  std::unique_ptr<Time> time_;
  std::unique_ptr<resource::ResourceManager> resource_manager_;
  std::unique_ptr<resource::AudioManager> audio_manager_;
  std::unique_ptr<render::Renderer> renderer_;
  std::unique_ptr<render::Camera> camera_;
  std::unique_ptr<render::TextRenderer> text_renderer_;
  std::unique_ptr<core::Config> config_;
  std::unique_ptr<input::InputManager> input_manager_;
  std::unique_ptr<physics::PhysicsEngine> physics_engine_;
  std::unique_ptr<core::GameState> game_state_;
  std::unique_ptr<core::Context> context_;

  std::unique_ptr<scene::SceneManager> scene_manager_;
};

}  // namespace engine::core
