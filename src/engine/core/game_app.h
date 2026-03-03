// Copyright Sylar129

#pragma once

#include <memory>

#include "engine/core/time.h"
struct SDL_Window;
struct SDL_Renderer;

namespace engine::resource {
class ResourceManager;
}

namespace engine::core {

class GameApp final {
 public:
  GameApp();
  ~GameApp();

  GameApp(const GameApp&) = delete;
  GameApp& operator=(const GameApp&) = delete;
  GameApp(GameApp&&) = delete;
  GameApp& operator=(GameApp&&) = delete;

  void Run();

 private:
  [[nodiscard]] bool Init();
  bool InitSDL();
  bool InitTime();
  bool InitResourceManager();

  void HandleEvents();
  void Update(float delta_time);
  void Render();
  void Close();

  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  bool is_running_ = false;
  std::unique_ptr<Time> time_;
  std::unique_ptr<resource::ResourceManager> resource_manager_;
};

}  // namespace engine::core
