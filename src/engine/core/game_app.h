// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/core/config.h"
#include "engine/core/time.h"
#include "engine/input/input_manager.h"
#include "engine/render/text_renderer.h"
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
  [[nodiscard]] bool Init();
  [[nodiscard]] bool InitConfig();
  [[nodiscard]] bool InitSDL();
  [[nodiscard]] bool InitRenderer();
  [[nodiscard]] bool InitCamera();
  [[nodiscard]] bool InitTextRenderer();
  [[nodiscard]] bool InitTime();
  [[nodiscard]] bool InitResourceManager();
  [[nodiscard]] bool InitInputManager();
  [[nodiscard]] bool InitGameState();
  [[nodiscard]] bool InitContext();
  [[nodiscard]] bool InitSceneManager();
  [[nodiscard]] bool InitPhysicsEngine();

  void HandleEvents();
  void Update(float delta_time);
  void Render();
  void Close();

  SDL_Window* window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;
  bool is_running_ = false;
  std::unique_ptr<Time> time_;
  std::unique_ptr<resource::ResourceManager> resource_manager_;
  std::unique_ptr<engine::render::Renderer> renderer_;
  std::unique_ptr<engine::render::Camera> camera_;
  std::unique_ptr<engine::render::TextRenderer> text_renderer_;
  std::unique_ptr<engine::core::Config> config_;
  std::unique_ptr<engine::input::InputManager> input_manager_;
  std::unique_ptr<engine::physics::PhysicsEngine> physics_engine_;
  std::unique_ptr<engine::core::GameState> game_state_;
  std::unique_ptr<engine::core::Context> context_;
  std::unique_ptr<engine::scene::SceneManager> scene_manager_;
};

}  // namespace engine::core
