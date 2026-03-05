// Copyright Sylar129

#include "engine/core/context.h"

#include "engine/input/input_manager.h"
#include "engine/render/camera.h"
#include "engine/render/renderer.h"
#include "engine/resource/resource_manager.h"
#include "log.h"

namespace engine::core {

Context::Context(engine::input::InputManager& input_manager,
                 engine::render::Renderer& renderer,
                 engine::render::Camera& camera,
                 engine::resource::ResourceManager& resource_manager)
    : input_manager_(input_manager),
      renderer_(renderer),
      camera_(camera),
      resource_manager_(resource_manager) {
  ENGINE_TRACE("Context has been initialized.");
}

}  // namespace engine::core
