// Copyright Sylar129

#pragma once

#include "utils/non_copyable.h"

namespace engine::core {
class Context;
}

namespace engine::object {
class GameObject;
}

namespace engine::component {

class Component {
  friend class object::GameObject;

 public:
  Component() = default;
  virtual ~Component() = default;

  // Subclass by deault is also non-copyable and non-movable
  DISABLE_COPY_AND_MOVE(Component);

  void SetOwner(object::GameObject* owner) { owner_ = owner; }
  [[nodiscard]] object::GameObject* GetOwner() const { return owner_; }

 protected:
  virtual void Init() {}
  virtual void HandleInput(core::Context& /* context */) {}
  virtual void Update(float /* delta_time */, core::Context& /* context */) {}
  virtual void Render(core::Context& /* context */) {}
  virtual void Clean() {}

  object::GameObject* owner_ = nullptr;
};

}  // namespace engine::component
