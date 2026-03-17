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
  friend class engine::object::GameObject;

 public:
  Component() = default;
  virtual ~Component() = default;

  // Subclass by deault is also non-copyable and non-movable
  DISABLE_COPY_AND_MOVE(Component);

  void SetOwner(engine::object::GameObject* owner) { owner_ = owner; }
  [[nodiscard]] engine::object::GameObject* GetOwner() const { return owner_; }

 protected:
  virtual void Init() {}
  virtual void HandleInput(engine::core::Context& /* context */) {}
  virtual void Update(float /* delta_time */,
                      engine::core::Context& /* context */) {}
  virtual void Render(engine::core::Context& /* context */) {}
  virtual void Clean() {}

  engine::object::GameObject* owner_ = nullptr;
};

}  // namespace engine::component
