// Copyright Sylar129

#include "engine/core/context.h"

#include "engine/input/input_manager.h"
#include "engine/render/camera.h"
#include "engine/render/renderer.h"
#include "engine/resource/resource_manager.h"
#include "utils/log.h"

namespace engine::core {

Context::Context(engine::input::InputManager& input_manager,
                 engine::render::Renderer& renderer,
                 engine::render::Camera& camera,
                 engine::render::TextRenderer& text_renderer,
                 engine::resource::ResourceManager& resource_manager,
                 engine::physics::PhysicsEngine& physics_engine,
                 engine::core::GameState& game_state)
    : input_manager_(input_manager),
      renderer_(renderer),
      camera_(camera),
      text_renderer_(text_renderer),
      resource_manager_(resource_manager),
      physics_engine_(physics_engine),
      game_state_(game_state) {
  ENGINE_LOG_TRACE("Context has been initialized.");
}

}  // namespace engine::core
