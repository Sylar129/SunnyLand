// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include "engine/component/component.h"
#include "log.h"

namespace engine::object {

class GameObject final {
 public:
  GameObject(const std::string& name, const std::string& tag = "");

  GameObject(const GameObject&) = delete;
  GameObject& operator=(const GameObject&) = delete;
  GameObject(GameObject&&) = delete;
  GameObject& operator=(GameObject&&) = delete;

  void SetName(const std::string& name) { name_ = name; }
  const std::string& GetName() const { return name_; }
  void SetTag(const std::string& tag) { tag_ = tag; }
  const std::string& GetTag() const { return tag_; }
  void SetNeedRemove(bool need_remove) { need_remove_ = need_remove; }
  bool IsNeedRemove() const { return need_remove_; }

  template <typename T, typename... Args>
  T* AddComponent(Args&&... args) {
    static_assert(std::is_base_of_v<engine::component::Component, T>,
                  "T has to be the subclass of Component");
    auto type_index = std::type_index(typeid(T));
    if (HasComponent<T>()) {
      return GetComponent<T>();
    }

    auto new_component = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = new_component.get();
    new_component->SetOwner(this);
    components_[type_index] = std::move(new_component);
    ptr->Init();
    ENGINE_DEBUG("GameObject::addComponent: {} added component {}", name_,
                 typeid(T).name());
    return ptr;
  }

  template <typename T>
  T* GetComponent() const {
    static_assert(std::is_base_of_v<engine::component::Component, T>,
                  "T has to be the subclass of Component");
    auto type_index = std::type_index(typeid(T));
    auto it = components_.find(type_index);
    if (it != components_.end()) {
      return static_cast<T*>(it->second.get());
    }
    return nullptr;
  }

  template <typename T>
  bool HasComponent() const {
    static_assert(std::is_base_of_v<engine::component::Component, T>,
                  "T has to be the subclass of Component");
    return components_.contains(std::type_index(typeid(T)));
  }

  template <typename T>
  void RemoveComponent() {
    static_assert(std::is_base_of_v<engine::component::Component, T>,
                  "T has to be the subclass of Component");
    auto type_index = std::type_index(typeid(T));
    auto it = components_.find(type_index);
    if (it != components_.end()) {
      it->second->Clean();
      components_.erase(it);
    }
  }

  void Update(float delta_time);
  void Render();
  void Clean();
  void HandleInput();

 private:
  std::string name_;
  std::string tag_;
  std::unordered_map<std::type_index,
                     std::unique_ptr<engine::component::Component>>
      components_;
  bool need_remove_ = false;
};

}  // namespace engine::object