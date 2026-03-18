// Copyright Sylar129

#include "engine/core/context.h"

#include "engine/input/input_manager.h"
#include "engine/render/camera.h"
#include "engine/render/renderer.h"
#include "engine/resource/resource_manager.h"
#include "utils/log.h"

namespace engine::core {

Context::Context(input::InputManager& input_manager, render::Renderer& renderer,
                 render::Camera& camera, render::TextRenderer& text_renderer,
                 resource::ResourceManager& resource_manager,
                 resource::AudioManager& audio_manager,
                 physics::PhysicsEngine& physics_engine,
                 core::GameState& game_state)
    : input_manager_(input_manager),
      renderer_(renderer),
      camera_(camera),
      text_renderer_(text_renderer),
      resource_manager_(resource_manager),
      audio_manager_(audio_manager),
      physics_engine_(physics_engine),
      game_state_(game_state) {
  ENGINE_LOG_TRACE("Context has been initialized.");
}

}  // namespace engine::core
