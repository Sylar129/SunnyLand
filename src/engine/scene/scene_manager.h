// Copyright Sylar129

#pragma once

#include <memory>
#include <vector>

#include "engine/utils/non_copyable.h"

namespace engine::core {
class Context;
}
namespace engine::scene {
class Scene;
}

namespace engine::scene {

class SceneManager final {
 public:
  explicit SceneManager(engine::core::Context& context);
  ~SceneManager();
  DISABLE_COPY_AND_MOVE(SceneManager);

  void RequestPushScene(std::unique_ptr<Scene>&& scene);
  void RequestPopScene();
  void RequestReplaceScene(std::unique_ptr<Scene>&& scene);

  Scene* GetCurrentScene() const;

  void Update(float delta_time);
  void Render();
  void HandleInput();
  void Close();

 private:
  void ProcessPendingActions();

  void PushScene(std::unique_ptr<Scene>&& scene);
  void PopScene();
  void ReplaceScene(std::unique_ptr<Scene>&& scene);

  engine::core::Context& context_;
  std::vector<std::unique_ptr<Scene>> scene_stack_;

  enum class PendingAction { None, Push, Pop, Replace };
  PendingAction pending_action_ = PendingAction::None;
  std::unique_ptr<Scene> pending_scene_;
};

}  // namespace engine::scene
