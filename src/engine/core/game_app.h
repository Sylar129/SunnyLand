// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/core/config.h"
#include "engine/core/time.h"
#include "engine/input/input_manager.h"
#include "engine/utils/non_copyable.h"

struct SDL_Window;
struct SDL_Renderer;

namespace engine::render {
class Renderer;
class Camera;
}  // namespace engine::render

namespace engine::resource {
class ResourceManager;
}

namespace engine::core {

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
  [[nodiscard]] bool InitTime();
  [[nodiscard]] bool InitResourceManager();
  [[nodiscard]] bool InitInputManager();

  void HandleEvents();
  void Update(float delta_time);
  void Render();
  void Close();

  // Test functions
  void TestRenderer();
  void TestCamera();
  void TestInputManager();

  SDL_Window* window_ = nullptr;
  SDL_Renderer* sdl_renderer_ = nullptr;
  bool is_running_ = false;
  std::unique_ptr<Time> time_;
  std::unique_ptr<resource::ResourceManager> resource_manager_;
  std::unique_ptr<engine::render::Renderer> renderer_;
  std::unique_ptr<engine::render::Camera> camera_;
  std::unique_ptr<engine::core::Config> config_;
  std::unique_ptr<engine::input::InputManager> input_manager_;
};

}  // namespace engine::core
