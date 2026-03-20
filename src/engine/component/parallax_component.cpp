// Copyright Sylar129

#include "engine/component/parallax_component.h"

#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/object/game_object.h"
#include "engine/render/renderer.h"
#include "engine/render/sprite.h"
#include "utils/assert.h"
#include "utils/log.h"

namespace engine::component {

ParallaxComponent::ParallaxComponent(const std::string& texture_id,
                                     const glm::vec2& scroll_factor,
                                     const glm::bvec2& repeat)
    : sprite_(render::Sprite(texture_id)),
      scroll_factor_(scroll_factor),
      repeat_(repeat) {
  ENGINE_LOG_TRACE("ParallaxComponent constructing finished. texture ID: {}",
                   texture_id);
}

void ParallaxComponent::Init() {
  ENGINE_LOG_ASSERT(owner_, "ParallaxComponent: owner is nullptr");

}

void ParallaxComponent::Render(core::Context& context) {
  if (is_hidden_) {
    return;
  }
  context.GetRenderer().DrawParallax(
      context.GetCamera(), sprite_,
      owner_->GetComponent<TransformComponent>().GetPosition(), scroll_factor_,
      repeat_, owner_->GetComponent<TransformComponent>().GetScale());
}

}  // namespace engine::component
