// Copyright Sylar129

#pragma once

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

namespace engine::core {

class Context final {
 public:
  Context(engine::input::InputManager& input_manager,
          engine::render::Renderer& renderer, engine::render::Camera& camera,
          engine::resource::ResourceManager& resource_manager);

  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;
  Context(Context&&) = delete;
  Context& operator=(Context&&) = delete;

  engine::input::InputManager& getInputManager() const {
    return input_manager_;
  }
  engine::render::Renderer& getRenderer() const { return renderer_; }
  engine::render::Camera& getCamera() const { return camera_; }
  engine::resource::ResourceManager& getResourceManager() const {
    return resource_manager_;
  }

 private:
  engine::input::InputManager& input_manager_;
  engine::render::Renderer& renderer_;
  engine::render::Camera& camera_;
  engine::resource::ResourceManager& resource_manager_;
};

}  // namespace engine::core
