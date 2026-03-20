// Copyright Sylar129

#include "engine/component/transform_component.h"

#include "engine/component/sprite_component.h"
#include "engine/object/game_object.h"

namespace engine::component {

void TransformComponent::SetScale(const glm::vec2& scale) {
  scale_ = scale;
  if (owner_) {
    if (owner_->HasComponent<SpriteComponent>()) {
      owner_->GetComponent<SpriteComponent>().UpdateOffset();
    }
  }
}

}  // namespace engine::component
