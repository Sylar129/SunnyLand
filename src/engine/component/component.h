// Copyright Sylar129

#pragma once

#include "engine/utils/non_copyable.h"

namespace engine::object {
class GameObject;
}

namespace engine::component {

class Component {
  friend class engine::object::GameObject;

 public:
  Component() = default;
  virtual ~Component() = default;

 private:
  DISABLE_COPY_AND_MOVE(Component);

 public:
  void SetOwner(engine::object::GameObject* owner) { owner_ = owner; }
  engine::object::GameObject* GetOwner() const { return owner_; }

 protected:
  virtual void Init() {}
  virtual void HandleInput() {}
  virtual void Update(float delta_time) {}
  virtual void Render() {}
  virtual void Clean() {}

 protected:
  engine::object::GameObject* owner_ = nullptr;
};

}  // namespace engine::component