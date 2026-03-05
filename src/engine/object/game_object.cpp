// Copyright Sylar129

#include "engine/object/game_object.h"

#include "log.h"

namespace engine::object {

GameObject::GameObject(const std::string& name, const std::string& tag)
    : name_(name), tag_(tag) {
  ENGINE_TRACE("GameObject created: {} {}", name_, tag_);
}

void GameObject::Update(float delta_time) {
  for (auto& pair : components_) {
    pair.second->Update(delta_time);
  }
}

void GameObject::Render() {
  for (auto& pair : components_) {
    pair.second->Render();
  }
}

void GameObject::Clean() {
  ENGINE_TRACE("Cleaning GameObject...");
  for (auto& pair : components_) {
    pair.second->Clean();
  }
  components_.clear();
}

void GameObject::HandleInput() {
  for (auto& pair : components_) {
    pair.second->HandleInput();
  }
}

}  // namespace engine::object
