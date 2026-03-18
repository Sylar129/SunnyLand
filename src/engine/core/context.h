// Copyright Sylar129

#pragma once

#include "engine/core/game_app.h"
#include "engine/render/text_renderer.h"
#include "engine/resource/audio_manager.h"
#include "utils/non_copyable.h"

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
class AudioManager;
}  // namespace engine::resource

namespace engine::physics {
class PhysicsEngine;
}

namespace engine::core {

class GameState;

class Context final {
 public:
  Context(input::InputManager& input_manager, render::Renderer& renderer,
          render::Camera& camera, render::TextRenderer& text_renderer,
          resource::ResourceManager& resource_manager,
          resource::AudioManager& audio_manager,
          physics::PhysicsEngine& physics_engine, core::GameState& game_state);
  DISABLE_COPY_AND_MOVE(Context);

  input::InputManager& GetInputManager() const { return input_manager_; }
  render::Renderer& GetRenderer() const { return renderer_; }
  render::Camera& GetCamera() const { return camera_; }
  render::TextRenderer& GetTextRenderer() const { return text_renderer_; }
  resource::ResourceManager& GetResourceManager() const {
    return resource_manager_;
  }
  resource::AudioManager& GetAudioManager() const { return audio_manager_; }
  physics::PhysicsEngine& GetPhysicsEngine() const { return physics_engine_; }
  core::GameState& GetGameState() const { return game_state_; }

 private:
  input::InputManager& input_manager_;
  render::Renderer& renderer_;
  render::Camera& camera_;
  render::TextRenderer& text_renderer_;
  resource::ResourceManager& resource_manager_;
  resource::AudioManager& audio_manager_;
  physics::PhysicsEngine& physics_engine_;
  core::GameState& game_state_;
};

}  // namespace engine::core
