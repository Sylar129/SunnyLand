// Copyright Sylar129

#include "engine/component/transform_component.h"

#include "engine/component/sprite_component.h"
#include "engine/object/game_object.h"

namespace engine::component {

void TransformComponent::SetScale(const glm::vec2& scale) {
  scale_ = scale;
  if (owner_) {
    auto sprite_comp = owner_->GetComponent<SpriteComponent>();
    if (sprite_comp) {
      sprite_comp->UpdateOffset();
    }
  }
}

}  // namespace engine::component
