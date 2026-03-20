// Copyright Sylar129

#include "engine/physics/physics_engine.h"

#include <set>

#include "engine/component/collider_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/tilelayer_component.h"
#include "engine/component/transform_component.h"
#include "glm/common.hpp"
#include "utils/assert.h"
#include "utils/collision.h"
#include "utils/log.h"

namespace engine::physics {

void PhysicsEngine::RegisterComponent(component::PhysicsComponent* component) {
  ENGINE_LOG_ASSERT(component, "PhysicsComponent is null.");
  components_.push_back(component);
  ENGINE_LOG_TRACE("Register PhysicsComponent complete.");
}

void PhysicsEngine::UnregisterComponent(
    component::PhysicsComponent* component) {
  std::erase(components_, component);
  ENGINE_LOG_TRACE("Unregister PhysicsComponent complete.");
}

void PhysicsEngine::RegisterCollisionLayer(
    component::TileLayerComponent* layer) {
  layer->SetPhysicsEngine(this);
  collision_tile_layers_.push_back(layer);
  ENGINE_LOG_TRACE("Register collision tile layer complete.");
}

void PhysicsEngine::UnregisterCollisionLayer(
    component::TileLayerComponent* layer) {
  std::erase(collision_tile_layers_, layer);
  ENGINE_LOG_TRACE("Unregister collision tile layer complete.");
}

void PhysicsEngine::Update(float delta_time) {
  collision_pairs_.clear();
  tile_trigger_events_.clear();

  for (auto* pc : components_) {
    if (!pc->IsEnabled()) {
      continue;
    }

    pc->ResetCollisionFlag();

    if (pc->IsUseGravity()) {
      pc->AddForce(gravity_ * pc->GetMass());
    }

    pc->velocity_ += (pc->GetForce() / pc->GetMass()) * delta_time;
    pc->velocity_ = glm::clamp(pc->velocity_, -max_speed_, max_speed_);
    pc->ClearForce();

    ApplyWorldBounds(pc);
  }
}

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

void PhysicsEngine::ApplyWorldBounds(component::PhysicsComponent* pc) {
  if (!pc || !world_bounds_) return;

  auto* obj = pc->GetOwner();
  auto& cc = obj->GetComponent<component::ColliderComponent>();
  auto& tc = obj->GetComponent<component::TransformComponent>();
  auto world_aabb = cc.GetWorldAABB();
  auto obj_pos = world_aabb.position;
  auto obj_size = world_aabb.size;

  if (obj_pos.x < world_bounds_->position.x) {
    pc->velocity_.x = 0.0f;
    obj_pos.x = world_bounds_->position.x;
    pc->SetCollidedLeft();
  }
  if (obj_pos.y < world_bounds_->position.y) {
    pc->velocity_.y = 0.0f;
    obj_pos.y = world_bounds_->position.y;
    pc->SetCollidedAbove();
  }
  if (obj_pos.x + obj_size.x >
      world_bounds_->position.x + world_bounds_->size.x) {
    pc->velocity_.x = 0.0f;
    obj_pos.x = world_bounds_->position.x + world_bounds_->size.x - obj_size.x;
    pc->SetCollidedRight();
  }
  tc.Translate(obj_pos - world_aabb.position);
}

}  // namespace engine::physics
