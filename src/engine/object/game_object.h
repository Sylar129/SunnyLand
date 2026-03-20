// Copyright Sylar129

#pragma once

#include <string>
#include <utility>

#include "entt/entt.hpp"

namespace engine::object {

class GameObject final {
 public:
  GameObject(entt::registry* registry);

  template <typename T, typename... Args>
  T& AddComponent(Args&&... args) {
    return registry_->emplace<T>(handle_, std::forward<Args>(args)...);
  }

  template <typename T>
  T& GetComponent() const {
    return registry_->get<T>(handle_);
  }

  template <typename T>
  bool HasComponent() const {
    return registry_->any_of<T>(handle_);
  }

  template <typename T>
  void RemoveComponent() {
    registry_->remove<T>(handle_);
  }

  std::string GetName() const;

 private:
  entt::registry* registry_;
  entt::entity handle_{};
};

}  // namespace engine::object
