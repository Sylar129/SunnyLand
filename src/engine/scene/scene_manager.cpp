// Copyright Sylar129

#include "engine/scene/scene_manager.h"

#include "engine/core/context.h"
#include "engine/scene/scene.h"
#include "log.h"

namespace engine::scene {

SceneManager::SceneManager(engine::core::Context& context) : context_(context) {
  ENGINE_LOG_TRACE("SceneManager initialized successfully.");
}

SceneManager::~SceneManager() {
  ENGINE_LOG_TRACE(
      "SceneManager is being destroyed. Cleaning up scene stack...");
  Close();
}

Scene* SceneManager::GetCurrentScene() const {
  if (scene_stack_.empty()) {
    return nullptr;
  }
  return scene_stack_.back().get();
}

void SceneManager::Update(float delta_time) {
  Scene* current_scene = GetCurrentScene();
  if (current_scene) {
    current_scene->Update(delta_time);
  }
  ProcessPendingActions();
}

void SceneManager::Render() {
  for (const auto& scene : scene_stack_) {
    scene->Render();
  }
}

void SceneManager::HandleInput() {
  Scene* current_scene = GetCurrentScene();
  if (current_scene) {
    current_scene->HandleInput();
  }
}

void SceneManager::Close() {
  ENGINE_LOG_TRACE("Closing SceneManager and cleaning up all scenes...");
  while (!scene_stack_.empty()) {
    if (scene_stack_.back()) {
      ENGINE_LOG_DEBUG("Cleaning scene '{}'", scene_stack_.back()->GetName());
      scene_stack_.back()->Clean();
    }
    scene_stack_.pop_back();
  }
}

void SceneManager::RequestPopScene() { pending_action_ = PendingAction::Pop; }

void SceneManager::RequestReplaceScene(std::unique_ptr<Scene>&& scene) {
  pending_action_ = PendingAction::Replace;
  pending_scene_ = std::move(scene);
}

void SceneManager::RequestPushScene(std::unique_ptr<Scene>&& scene) {
  pending_action_ = PendingAction::Push;
  pending_scene_ = std::move(scene);
}

void SceneManager::ProcessPendingActions() {
  if (pending_action_ == PendingAction::None) {
    return;
  }

  switch (pending_action_) {
    case PendingAction::Pop:
      PopScene();
      break;
    case PendingAction::Replace:
      ReplaceScene(std::move(pending_scene_));
      break;
    case PendingAction::Push:
      PushScene(std::move(pending_scene_));
      break;
    default:
      break;
  }

  pending_action_ = PendingAction::None;
}

void SceneManager::PushScene(std::unique_ptr<Scene>&& scene) {
  if (!scene) {
    ENGINE_LOG_WARN("Trying to push an empty scene.");
    return;
  }
  ENGINE_LOG_DEBUG("Pushing scene '{}' to stack.", scene->GetName());

  if (!scene->IsInitialized()) {
    scene->Init();
  }

  scene_stack_.push_back(std::move(scene));
}

void SceneManager::PopScene() {
  if (scene_stack_.empty()) {
    ENGINE_LOG_WARN("Trying to pop scene from an empty stack.");
    return;
  }
  ENGINE_LOG_DEBUG("Poping scene '{}' from stack.",
                   scene_stack_.back()->GetName());

  if (scene_stack_.back()) {
    scene_stack_.back()->Clean();
  }
  scene_stack_.pop_back();
}

void SceneManager::ReplaceScene(std::unique_ptr<Scene>&& scene) {
  if (!scene) {
    ENGINE_LOG_WARN("Trying to replace with an empty scene.");
    return;
  }
  ENGINE_LOG_DEBUG("Replacing scene '{}' with scene '{}'.",
                   scene_stack_.back()->GetName(), scene->GetName());

  while (!scene_stack_.empty()) {
    if (scene_stack_.back()) {
      scene_stack_.back()->Clean();
    }
    scene_stack_.pop_back();
  }

  if (!scene->IsInitialized()) {
    scene->Init();
  }

  scene_stack_.push_back(std::move(scene));
}

}  // namespace engine::scene
