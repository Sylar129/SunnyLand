// Copyright Sylar129

#pragma once

#include "engine/utils/non_copyable.h"

namespace engine::input {
class InputManager;
}

namespace engine::render {
class Renderer;
class Camera;
}  // namespace engine::render

namespace engine::resource {
class ResourceManager;
}

namespace engine::physics {
class PhysicsEngine;
}

namespace engine::core {

class Context final {
 public:
  Context(engine::input::InputManager& input_manager,
          engine::render::Renderer& renderer, engine::render::Camera& camera,
          engine::resource::ResourceManager& resource_manager,
          engine::physics::PhysicsEngine& physics_engine);
  DISABLE_COPY_AND_MOVE(Context);

  engine::input::InputManager& getInputManager() const {
    return input_manager_;
  }
  engine::render::Renderer& getRenderer() const { return renderer_; }
  engine::render::Camera& getCamera() const { return camera_; }
  engine::resource::ResourceManager& getResourceManager() const {
    return resource_manager_;
  }
  engine::physics::PhysicsEngine& getPhysicsEngine() const {
    return physics_engine_;
  }

 private:
  engine::input::InputManager& input_manager_;
  engine::render::Renderer& renderer_;
  engine::render::Camera& camera_;
  engine::resource::ResourceManager& resource_manager_;
  engine::physics::PhysicsEngine& physics_engine_;
};

}  // namespace engine::core
