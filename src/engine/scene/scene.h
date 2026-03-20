// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "entt/entt.hpp"
#include "utils/non_copyable.h"

namespace engine::core {
class Context;
}

namespace engine::ui {
class UIManager;

}

namespace engine::scene {
class SceneManager;

class Scene {
 public:
  Scene(const std::string& name, core::Context& context,
        SceneManager& scene_manager);

  virtual ~Scene();
  DISABLE_COPY_AND_MOVE(Scene);

  virtual void Init();
  virtual void Update(float delta_time);
  virtual void Render();
  virtual void HandleInput();
  virtual void Clean();

  void SetName(const std::string& name) { scene_name_ = name; }
  const std::string& GetName() const { return scene_name_; }
  void SetInitialized(bool initialized) { is_initialized_ = initialized; }
  bool IsInitialized() const { return is_initialized_; }

  core::Context& GetContext() const { return context_; }
  SceneManager& GetSceneManager() const { return scene_manager_; }

 protected:
  std::string scene_name_;
  core::Context& context_;
  SceneManager& scene_manager_;
  std::unique_ptr<ui::UIManager> ui_manager_;
  bool is_initialized_ = false;
  entt::registry registry_;
};

}  // namespace engine::scene
