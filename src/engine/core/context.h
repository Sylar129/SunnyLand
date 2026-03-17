// Copyright Sylar129

#pragma once

#include "engine/render/text_renderer.h"
#include "engine/utils/non_copyable.h"

namespace engine::input {
class InputManager;
}

namespace engine::render {
class Renderer;
class Camera;
class TextRenderer;
}  // namespace engine::render

namespace engine::resource {
class ResourceManager;
}

namespace engine::physics {
class PhysicsEngine;
}

namespace engine::core {

class GameState;

class Context final {
 public:
  Context(engine::input::InputManager& input_manager,
          engine::render::Renderer& renderer, engine::render::Camera& camera,
          engine::render::TextRenderer& text_renderer,
          engine::resource::ResourceManager& resource_manager,
          engine::physics::PhysicsEngine& physics_engine,
          engine::core::GameState& game_state);
  DISABLE_COPY_AND_MOVE(Context);

  engine::input::InputManager& GetInputManager() const {
    return input_manager_;
  }
  engine::render::Renderer& GetRenderer() const { return renderer_; }
  engine::render::Camera& GetCamera() const { return camera_; }
  engine::render::TextRenderer& GetTextRenderer() const {
    return text_renderer_;
  }
  engine::resource::ResourceManager& GetResourceManager() const {
    return resource_manager_;
  }
  engine::physics::PhysicsEngine& GetPhysicsEngine() const {
    return physics_ENGINE_LOG_;
  }
  engine::core::GameState& GetGameState() const { return game_state_; }

 private:
  engine::input::InputManager& input_manager_;
  engine::render::Renderer& renderer_;
  engine::render::Camera& camera_;
  engine::render::TextRenderer& text_renderer_;
  engine::resource::ResourceManager& resource_manager_;
  engine::physics::PhysicsEngine& physics_ENGINE_LOG_;
  engine::core::GameState& game_state_;
};

}  // namespace engine::core
