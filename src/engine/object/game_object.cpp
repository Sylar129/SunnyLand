// Copyright Sylar129

#include "engine/object/game_object.h"

#include "engine/component/tag_component.h"

namespace engine::object {

GameObject::GameObject(entt::registry* registry)
    : registry_(registry), handle_(registry_->create()) {}

std::string GameObject::GetName() const {
  if (HasComponent<TagComponent>()) {
    return GetComponent<TagComponent>().name;
  }
  return "GameObject_" + std::to_string((uint32_t)handle_);
}

}  // namespace engine::object
