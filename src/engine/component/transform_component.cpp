// Copyright Sylar129

#include "engine/component/transform_component.h"

namespace engine::component {

void TransformComponent::setScale(const glm::vec2 &scale) {
  scale_ = scale;
}

}  // namespace engine::component