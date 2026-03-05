// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "engine/utils/non_copyable.h"

namespace engine::core {
class Context;
}

namespace engine::object {
class GameObject;
}

namespace engine::scene {
class SceneManager;

class Scene {
 protected:
  std::string scene_name_;          ///< @brief 场景名称
  engine::core::Context& context_;  ///< @brief 上下文引用（隐式，构造时传入）
  engine::scene::SceneManager&
      scene_manager_;  ///< @brief 场景管理器引用（构造时传入）
  bool is_initialized_ =
      false;  ///< @brief
              ///< 场景是否已初始化(非当前场景很可能未被删除，因此需要初始化标志避免重复初始化)
  std::vector<std::unique_ptr<engine::object::GameObject>>
      game_objects_;  ///< @brief 场景中的游戏对象
  std::vector<std::unique_ptr<engine::object::GameObject>>
      pending_additions_;  ///< @brief 待添加的游戏对象（延时添加）

 public:
  /**
   * @brief 构造函数。
   *
   * @param name 场景的名称。
   * @param context 场景上下文。
   * @param scene_manager 场景管理器。
   */
  Scene(std::string name, engine::core::Context& context,
        engine::scene::SceneManager& scene_manager);

  virtual ~Scene();
  DISABLE_COPY_AND_MOVE(Scene);

  virtual void Init();
  virtual void Update(float delta_time);
  virtual void Render();
  virtual void HandleInput();
  virtual void Clean();

  /// @brief 直接向场景中添加一个游戏对象。（初始化时可用，游戏进行中不安全）
  /// （&&表示右值引用，与std::move搭配使用，避免拷贝）
  virtual void AddGameObject(
      std::unique_ptr<engine::object::GameObject>&& game_object);

  /// @brief 安全地添加游戏对象。（添加到pending_additions_中）
  virtual void SafeAddGameObject(
      std::unique_ptr<engine::object::GameObject>&& game_object);

  /// @brief 直接从场景中移除一个游戏对象。（一般不使用，但保留实现的逻辑）
  virtual void RemoveGameObject(engine::object::GameObject* game_object_ptr);

  /// @brief 安全地移除游戏对象。（设置need_remove_标记）
  virtual void SafeRemoveGameObject(
      engine::object::GameObject* game_object_ptr);

  /// @brief 获取场景中的游戏对象容器。
  const std::vector<std::unique_ptr<engine::object::GameObject>>&
  GetGameObjects() const {
    return game_objects_;
  }

  /// @brief 根据名称查找游戏对象（返回找到的第一个对象）。
  engine::object::GameObject* FindGameObjectByName(
      const std::string& name) const;

  // getters and setters
  void SetName(const std::string& name) {
    scene_name_ = name;
  }  ///< @brief 设置场景名称
  const std::string& GetName() const {
    return scene_name_;
  }  ///< @brief 获取场景名称
  void SetInitialized(bool initialized) {
    is_initialized_ = initialized;
  }  ///< @brief 设置场景是否已初始化
  bool IsInitialized() const {
    return is_initialized_;
  }  ///< @brief 获取场景是否已初始化

  engine::core::Context& GetContext() const { return context_; }
  engine::scene::SceneManager& GetSceneManager() const {
    return scene_manager_;
  }
  std::vector<std::unique_ptr<engine::object::GameObject>>& GetGameObjects() {
    return game_objects_;
  }

 protected:
  void
  ProcessPendingAdditions();  ///< @brief
                              ///< 处理待添加的游戏对象。（每轮更新的最后调用）
};

}  // namespace engine::scene
