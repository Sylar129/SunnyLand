// Copyright Sylar129

#pragma once

#include <memory>

#include "utils/non_copyable.h"

struct SDL_Window;
struct SDL_Renderer;

namespace engine::scene {
class SceneManager;
}

namespace engine::core {

class Context;

class GameApp final {
 public:
  GameApp();
  ~GameApp();
  DISABLE_COPY_AND_MOVE(GameApp);

  bool Init();
  void Run();
  void Close();

 private:
  bool InitContext();
  bool InitSceneManager();

  void HandleEvents();
  void Update(float delta_time);
  void Render();

  std::unique_ptr<core::Context> context_;
  std::unique_ptr<scene::SceneManager> scene_manager_;
  bool is_running_ = false;
};

}  // namespace engine::core
