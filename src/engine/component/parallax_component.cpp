// Copyright Sylar129

#include "engine/component/parallax_component.h"

#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/object/game_object.h"
#include "engine/render/renderer.h"
#include "engine/render/sprite.h"
#include "engine/utils/assert.h"
#include "log.h"

namespace engine::component {

ParallaxComponent::ParallaxComponent(const std::string& texture_id,
                                     const glm::vec2& scroll_factor,
                                     const glm::bvec2& repeat)
    : sprite_(engine::render::Sprite(texture_id)),
      scroll_factor_(scroll_factor),
      repeat_(repeat) {
  ENGINE_TRACE("ParallaxComponent constructing finished. texture ID: {}", texture_id);
}

void ParallaxComponent::Init() {
  ENGINE_ASSERT(owner_, "ParallaxComponent: owner is nullptr");

  transform_ = owner_->GetComponent<TransformComponent>();
  if (!transform_) {
    ENGINE_ERROR("ParallaxComponent: can't find TransformComponent");
    return;
  }
}

void ParallaxComponent::Render(engine::core::Context& context) {
  if (is_hidden_ || !transform_) {
    return;
  }
  context.getRenderer().DrawParallax(context.getCamera(), sprite_,
                                     transform_->GetPosition(), scroll_factor_,
                                     repeat_, transform_->GetScale());
}

}  // namespace engine::component
