// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "utils/non_copyable.h"

namespace engine::core {
class Context;
}

namespace engine::ui {
class UIManager;

}

namespace engine::object {
class GameObject;
}

namespace engine::scene {
class SceneManager;

class Scene {
 public:
  Scene(const std::string& name, engine::core::Context& context,
        engine::scene::SceneManager& scene_manager);

  virtual ~Scene();
  DISABLE_COPY_AND_MOVE(Scene);

  virtual void Init();
  virtual void Update(float delta_time);
  virtual void Render();
  virtual void HandleInput();
  virtual void Clean();

  virtual void AddGameObject(
      std::unique_ptr<engine::object::GameObject>&& game_object);

  virtual void SafeAddGameObject(
      std::unique_ptr<engine::object::GameObject>&& game_object);

  virtual void RemoveGameObject(engine::object::GameObject* game_object_ptr);

  virtual void SafeRemoveGameObject(
      engine::object::GameObject* game_object_ptr);

  const std::vector<std::unique_ptr<engine::object::GameObject>>&
  GetGameObjects() const {
    return game_objects_;
  }

  engine::object::GameObject* FindGameObjectByName(
      const std::string& name) const;

  void SetName(const std::string& name) { scene_name_ = name; }
  const std::string& GetName() const { return scene_name_; }
  void SetInitialized(bool initialized) { is_initialized_ = initialized; }
  bool IsInitialized() const { return is_initialized_; }

  engine::core::Context& GetContext() const { return context_; }
  engine::scene::SceneManager& GetSceneManager() const {
    return scene_manager_;
  }
  std::vector<std::unique_ptr<engine::object::GameObject>>& GetGameObjects() {
    return game_objects_;
  }

 protected:
  void ProcessPendingAdditions();

  std::string scene_name_;
  engine::core::Context& context_;
  engine::scene::SceneManager& scene_manager_;
  std::unique_ptr<engine::ui::UIManager> ui_manager_;
  bool is_initialized_ = false;
  std::vector<std::unique_ptr<engine::object::GameObject>> game_objects_;
  std::vector<std::unique_ptr<engine::object::GameObject>> pending_additions_;
};

}  // namespace engine::scene
