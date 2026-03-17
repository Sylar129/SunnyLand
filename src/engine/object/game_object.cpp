// Copyright Sylar129

#include "engine/object/game_object.h"

#include "log.h"

namespace engine::object {

GameObject::GameObject(const std::string& name, const std::string& tag)
    : name_(name), tag_(tag) {
  ENGINE_LOG_TRACE("GameObject created: {} {}", name_, tag_);
}

void GameObject::HandleInput(engine::core::Context& context) {
  for (auto& pair : components_) {
    pair.second->HandleInput(context);
  }
}

void GameObject::Update(float delta_time, engine::core::Context& context) {
  for (auto& pair : components_) {
    pair.second->Update(delta_time, context);
  }
}

void GameObject::Render(engine::core::Context& context) {
  for (auto& pair : components_) {
    pair.second->Render(context);
  }
}

void GameObject::Clean() {
  ENGINE_LOG_TRACE("Cleaning GameObject...");
  for (auto& pair : components_) {
    pair.second->Clean();
  }
  components_.clear();
}

}  // namespace engine::object
