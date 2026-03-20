// Copyright Sylar129

#include "engine/physics/physics_engine.h"

#include "engine/component/physics_component.h"
#include "engine/component/tilelayer_component.h"
#include "glm/common.hpp"

namespace engine::physics {

float PhysicsEngine::GetTileHeightAtWidth(float width, component::TileType type,
                                          glm::vec2 tile_size) {
  auto rel_x = glm::clamp(width / tile_size.x, 0.0f, 1.0f);
  if (type == "slope_0_1") {
    return rel_x * tile_size.y;
  }
  if (type == "slope_0_2") {
    return rel_x * tile_size.y * 0.5f;
  }
  if (type == "slope_2_1") {
    return rel_x * tile_size.y * 0.5f + tile_size.y * 0.5f;
  }
  if (type == "slope_1_0") {
    return (1.0f - rel_x) * tile_size.y;
  }
  if (type == "slope_2_0") {
    return (1.0f - rel_x) * tile_size.y * 0.5f;
  }
  if (type == "slope_1_2") {
    return (1.0f - rel_x) * tile_size.y * 0.5f + tile_size.y * 0.5f;
  }
  return 0;
}

void PhysicsEngine::ApplyWorldBounds(component::PhysicsComponent* pc) {}

}  // namespace engine::physics
