// Copyright Sylar129

#pragma once

namespace engine::object {
class GameObject;
}

namespace engine::component {

class Component {
  friend class engine::object::GameObject;

 public:
  Component() = default;
  virtual ~Component() = default;

  Component(const Component&) = delete;
  Component& operator=(const Component&) = delete;
  Component(Component&&) = delete;
  Component& operator=(Component&&) = delete;

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