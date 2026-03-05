// Copyright Sylar129

#include "engine/scene/scene_manager.h"

#include "engine/core/context.h"
#include "engine/scene/scene.h"
#include "log.h"

namespace engine::scene {

SceneManager::SceneManager(engine::core::Context& context) : context_(context) {
  ENGINE_TRACE("场景管理器已创建。");
}

SceneManager::~SceneManager() {
  ENGINE_TRACE("场景管理器已销毁。");
  Close();  // 即使不手动调用 close 也能确保清理
}

Scene* SceneManager::GetCurrentScene() const {
  if (scene_stack_.empty()) {
    return nullptr;
  }
  return scene_stack_.back().get();  // 返回栈顶场景的裸指针
}

void SceneManager::Update(float delta_time) {
  // 只更新栈顶（当前）场景
  Scene* current_scene = GetCurrentScene();
  if (current_scene) {
    current_scene->Update(delta_time);
  }
  // 执行可能的切换场景操作
  ProcessPendingActions();
}

void SceneManager::Render() {
  // 渲染时需要叠加渲染所有场景，而不只是栈顶
  for (const auto& scene : scene_stack_) {
    if (scene) {
      scene->Render();
    }
  }
}

void SceneManager::HandleInput() {
  // 只考虑栈顶场景
  Scene* current_scene = GetCurrentScene();
  if (current_scene) {
    current_scene->HandleInput();
  }
}

void SceneManager::Close() {
  ENGINE_TRACE("正在关闭场景管理器并清理场景栈...");
  // 清理栈中所有剩余的场景（从顶到底）
  while (!scene_stack_.empty()) {
    if (scene_stack_.back()) {
      ENGINE_DEBUG("正在清理场景 '{}' 。", scene_stack_.back()->GetName());
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

// --- Private Methods ---

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
    ENGINE_WARN("尝试将空场景压入栈。");
    return;
  }
  ENGINE_DEBUG("正在将场景 '{}' 压入栈。", scene->GetName());

  // 初始化新场景
  if (!scene->IsInitialized()) {  // 确保只初始化一次
    scene->Init();
  }

  // 将新场景移入栈顶
  scene_stack_.push_back(std::move(scene));
}

void SceneManager::PopScene() {
  if (scene_stack_.empty()) {
    ENGINE_WARN("尝试从空场景栈中弹出。");
    return;
  }
  ENGINE_DEBUG("正在从栈中弹出场景 '{}' 。", scene_stack_.back()->GetName());

  // 清理并移除栈顶场景
  if (scene_stack_.back()) {
    scene_stack_.back()->Clean();  // 显式调用清理
  }
  scene_stack_.pop_back();
}

void SceneManager::ReplaceScene(std::unique_ptr<Scene>&& scene) {
  if (!scene) {
    ENGINE_WARN("尝试用空场景替换。");
    return;
  }
  ENGINE_DEBUG("正在用场景 '{}' 替换场景 '{}' 。", scene->GetName(),
               scene_stack_.back()->GetName());

  // 清理并移除场景栈中所有场景
  while (!scene_stack_.empty()) {
    if (scene_stack_.back()) {
      scene_stack_.back()->Clean();
    }
    scene_stack_.pop_back();
  }

  // 初始化新场景
  if (!scene->IsInitialized()) {
    scene->Init();
  }

  // 将新场景压入栈顶
  scene_stack_.push_back(std::move(scene));
}

}  // namespace engine::scene
