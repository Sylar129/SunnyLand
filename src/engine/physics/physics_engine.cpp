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

    ResolveTileCollisions(pc, delta_time);

    ApplyWorldBounds(pc);
  }

  CheckObjectCollisions();

  CheckTileTriggers();
}

void PhysicsEngine::CheckObjectCollisions() {
  for (size_t i = 0; i < components_.size(); ++i) {
    auto* pc_a = components_[i];
    if (!pc_a->IsEnabled()) continue;
    auto* obj_a = pc_a->GetOwner();
    if (!obj_a) continue;
    auto* cc_a = obj_a->GetComponent<component::ColliderComponent>();
    if (!cc_a || !cc_a->IsActive()) continue;

    for (size_t j = i + 1; j < components_.size(); ++j) {
      auto* pc_b = components_[j];
      if (!pc_b->IsEnabled()) continue;
      auto* obj_b = pc_b->GetOwner();
      if (!obj_b) continue;
      auto* cc_b = obj_b->GetComponent<component::ColliderComponent>();
      if (!cc_b || !cc_b->IsActive()) continue;

      if (CheckRectOverlap(cc_a->GetWorldAABB(), cc_b->GetWorldAABB())) {
        if (obj_a->GetTag() != "solid" && obj_b->GetTag() == "solid") {
          ResolveSolidObjectCollisions(obj_a, obj_b);
        } else if (obj_a->GetTag() == "solid" && obj_b->GetTag() != "solid") {
          ResolveSolidObjectCollisions(obj_b, obj_a);
        } else {
          collision_pairs_.emplace_back(obj_a, obj_b);
        }
      }
    }
  }
}

void PhysicsEngine::CheckTileTriggers() {
  for (auto* pc : components_) {
    if (!pc || !pc->IsEnabled()) continue;
    auto* obj = pc->GetOwner();
    if (!obj) continue;
    auto* cc = obj->GetComponent<component::ColliderComponent>();
    if (!cc || !cc->IsActive() || cc->IsTrigger()) continue;

    auto world_aabb = cc->GetWorldAABB();

    std::set<component::TileType> triggers_set;

    for (auto* layer : collision_tile_layers_) {
      if (!layer) continue;
      auto tile_size = layer->GetTileSize();
      constexpr float tolerance = 1.0f;
      auto start_x =
          static_cast<int>(floor(world_aabb.position.x / tile_size.x));
      auto end_x = static_cast<int>(
          ceil((world_aabb.position.x + world_aabb.size.x - tolerance) /
               tile_size.x));
      auto start_y =
          static_cast<int>(floor(world_aabb.position.y / tile_size.y));
      auto end_y = static_cast<int>(
          ceil((world_aabb.position.y + world_aabb.size.y - tolerance) /
               tile_size.y));

      for (int x = start_x; x < end_x; ++x) {
        for (int y = start_y; y < end_y; ++y) {
          auto tile_type = layer->GetTileTypeAt({x, y});
          if (tile_type == component::TileType::kHazard) {
            triggers_set.insert(tile_type);
          } else if (tile_type == component::TileType::kLadder) {
            pc->SetCollidedLadder();
          }
        }
      }
      for (const auto& type : triggers_set) {
        tile_trigger_events_.emplace_back(obj, type);
        ENGINE_LOG_TRACE(
            "tile_trigger_events_ Add GameObject {} with TileType {}",
            obj->GetName(), static_cast<int>(type));
      }
    }
  }
}

// Resolves collisions between a physics object and collision tile layers.
//
// Algorithm:
//   1. Validate component existence and collision eligibility
//   2. Calculate the object's displacement for this frame (velocity *
//   delta_time)
//   3. For each collision layer, check collisions in all four directions:
//      - Right (positive X): Check if object moving right collides with solid
//      tiles
//      - Left (negative X): Check if object moving left collides with solid
//      tiles
//      - Down (positive Y): Check if object moving down collides with solid
//      tiles
//      - Up (negative Y): Check if object moving up collides with solid tiles
//   4. When collision is detected, clamp the object's position to prevent
//      penetration and stop velocity in the collision axis
//   5. Update transform position
//
// Collision Detection:
//   - Checks two corners of the AABB perpendicular to movement direction
//   - Tolerance (1.0f) is used to handle floating-point precision at boundaries
//   - Only checks solid tiles, ignores other tile types
//
// Behavior:
//   - Detects continuous collision (movement-aware)
//   - Sets velocity to 0 when collision occurs (fully absorbs momentum)
//   - Processes multiple collision layers (checks all layers)
void PhysicsEngine::ResolveTileCollisions(component::PhysicsComponent* pc,
                                          float delta_time) {
  // Validate that the physics component has an owner object
  auto* obj = pc->GetOwner();
  if (!obj) return;

  // Get required components: Transform for position updates, Collider for
  // bounds
  auto* tc = obj->GetComponent<component::TransformComponent>();
  auto* cc = obj->GetComponent<component::ColliderComponent>();

  // Skip collision resolution if components missing, inactive, or is a trigger
  if (!tc || !cc || cc->IsTrigger()) return;

  // Get the world-space AABB of the collider
  auto world_aabb = cc->GetWorldAABB();
  auto obj_pos = world_aabb.position;
  auto obj_size = world_aabb.size;

  // Skip invalid AABB (zero or negative dimensions)
  if (world_aabb.size.x <= 0.0f || world_aabb.size.y <= 0.0f) return;

  // Tolerance is subtracted from bottom-right corners to handle floating-point
  // precision at tile boundaries, preventing incorrect collision detection
  auto tolerance = 1.0f;

  // Calculate displacement for this frame
  auto ds = pc->velocity_ * delta_time;
  // Calculate new position after applying displacement
  auto new_obj_pos = obj_pos + ds;

  if (!cc->IsActive()) {
    tc->Translate(ds);
    pc->velocity_ = glm::clamp(pc->velocity_, -max_speed_, max_speed_);
    return;
  }

  // Check collisions against each registered collision tile layer
  for (auto* layer : collision_tile_layers_) {
    if (!layer) continue;
    auto tile_size = layer->GetTileSize();

    // === HORIZONTAL COLLISION CHECK (X-axis) ===
    if (ds.x > 0.0f) {
      // Object moving RIGHT: check right edge of collider against solid tiles
      auto right_top_x = new_obj_pos.x + obj_size.x;
      auto tile_x = static_cast<int>(floor(right_top_x / tile_size.x));

      // Check top-right corner
      auto tile_y = static_cast<int>(floor(obj_pos.y / tile_size.y));
      auto tile_type_top = layer->GetTileTypeAt({tile_x, tile_y});

      // Check bottom-right corner (with tolerance to handle edge cases)
      auto tile_y_bottom = static_cast<int>(
          floor((obj_pos.y + obj_size.y - tolerance) / tile_size.y));
      auto tile_type_bottom = layer->GetTileTypeAt({tile_x, tile_y_bottom});

      if (tile_type_top == component::TileType::kSolid ||
          tile_type_bottom == component::TileType::kSolid) {
        new_obj_pos.x = tile_x * tile_size.x - obj_size.x;
        pc->velocity_.x = 0.0f;
        pc->SetCollidedRight();
      } else {
        auto width_right = new_obj_pos.x + obj_size.x - tile_x * tile_size.x;
        auto height_right =
            GetTileHeightAtWidth(width_right, tile_type_bottom, tile_size);
        if (height_right > 0.0f) {
          if (new_obj_pos.y >
              (tile_y_bottom + 1) * tile_size.y - obj_size.y - height_right) {
            new_obj_pos.y =
                (tile_y_bottom + 1) * tile_size.y - obj_size.y - height_right;
            pc->SetCollidedBelow();
          }
        }
      }
    } else if (ds.x < 0.0f) {
      // Object moving LEFT: check left edge of collider against solid tiles
      auto left_top_x = new_obj_pos.x;
      auto tile_x = static_cast<int>(floor(left_top_x / tile_size.x));

      // Check top-left corner
      auto tile_y = static_cast<int>(floor(obj_pos.y / tile_size.y));
      auto tile_type_top = layer->GetTileTypeAt({tile_x, tile_y});

      // Check bottom-left corner (with tolerance)
      auto tile_y_bottom = static_cast<int>(
          floor((obj_pos.y + obj_size.y - tolerance) / tile_size.y));
      auto tile_type_bottom = layer->GetTileTypeAt({tile_x, tile_y_bottom});

      if (tile_type_top == component::TileType::kSolid ||
          tile_type_bottom == component::TileType::kSolid) {
        new_obj_pos.x = (tile_x + 1) * tile_size.x;
        pc->velocity_.x = 0.0f;
        pc->SetCollidedLeft();
      } else {
        auto width_left = new_obj_pos.x - tile_x * tile_size.x;
        auto height_left =
            GetTileHeightAtWidth(width_left, tile_type_bottom, tile_size);
        if (height_left > 0.0f) {
          if (new_obj_pos.y >
              (tile_y_bottom + 1) * tile_size.y - obj_size.y - height_left) {
            new_obj_pos.y =
                (tile_y_bottom + 1) * tile_size.y - obj_size.y - height_left;
            pc->SetCollidedBelow();
          }
        }
      }
    }

    // === VERTICAL COLLISION CHECK (Y-axis) ===
    if (ds.y > 0.0f) {
      // Object moving DOWN: check bottom edge of collider against solid tiles
      auto bottom_left_y = new_obj_pos.y + obj_size.y;
      auto tile_y = static_cast<int>(floor(bottom_left_y / tile_size.y));

      // Check bottom-left corner
      auto tile_x = static_cast<int>(floor(obj_pos.x / tile_size.x));
      auto tile_type_left = layer->GetTileTypeAt({tile_x, tile_y});

      // Check bottom-right corner (with tolerance)
      auto tile_x_right = static_cast<int>(
          floor((obj_pos.x + obj_size.x - tolerance) / tile_size.x));
      auto tile_type_right = layer->GetTileTypeAt({tile_x_right, tile_y});

      if (tile_type_left == component::TileType::kSolid ||
          tile_type_right == component::TileType::kSolid ||
          tile_type_left == component::TileType::kUnisolid ||
          tile_type_right == component::TileType::kUnisolid) {
        new_obj_pos.y = tile_y * tile_size.y - obj_size.y;
        pc->velocity_.y = 0.0f;
        pc->SetCollidedBelow();
      } else if (tile_type_left == component::TileType::kLadder &&
                 tile_type_right == component::TileType::kLadder) {
        auto tile_type_up_l = layer->GetTileTypeAt({tile_x, tile_y - 1});
        auto tile_type_up_r = layer->GetTileTypeAt({tile_x_right, tile_y - 1});
        if (tile_type_up_r != component::TileType::kLadder &&
            tile_type_up_l != component::TileType::kLadder) {
          if (pc->IsUseGravity()) {
            pc->SetOnTopLadder();
            pc->SetCollidedBelow();
            new_obj_pos.y = tile_y * layer->GetTileSize().y - obj_size.y;
            pc->velocity_.y = 0.0f;
          } else {
            // climbing...
          }
        }
      } else {
        auto width_left = obj_pos.x - tile_x * tile_size.x;
        auto width_right = obj_pos.x + obj_size.x - tile_x_right * tile_size.x;
        auto height_left =
            GetTileHeightAtWidth(width_left, tile_type_left, tile_size);
        auto height_right =
            GetTileHeightAtWidth(width_right, tile_type_right, tile_size);
        auto height = glm::max(height_left, height_right);
        if (height > 0.0f) {
          if (new_obj_pos.y >
              (tile_y + 1) * tile_size.y - obj_size.y - height) {
            new_obj_pos.y = (tile_y + 1) * tile_size.y - obj_size.y - height;
            pc->velocity_.y = 0.0f;
            pc->SetCollidedBelow();
          }
        }
      }
    } else if (ds.y < 0.0f) {
      // Object moving UP: check top edge of collider against solid tiles
      auto top_left_y = new_obj_pos.y;
      auto tile_y = static_cast<int>(floor(top_left_y / tile_size.y));

      // Check top-left corner
      auto tile_x = static_cast<int>(floor(obj_pos.x / tile_size.x));
      auto tile_type_left = layer->GetTileTypeAt({tile_x, tile_y});

      // Check top-right corner (with tolerance)
      auto tile_x_right = static_cast<int>(
          floor((obj_pos.x + obj_size.x - tolerance) / tile_size.x));
      auto tile_type_right = layer->GetTileTypeAt({tile_x_right, tile_y});

      // If either corner hits a solid tile, clamp Y position and stop Y
      // velocity
      if (tile_type_left == component::TileType::kSolid ||
          tile_type_right == component::TileType::kSolid) {
        new_obj_pos.y = (tile_y + 1) * tile_size.y;
        pc->velocity_.y = 0.0f;
        pc->SetCollidedAbove();
      }
    }
  }

  // Apply the collision-adjusted position to the transform
  tc->Translate(new_obj_pos - obj_pos);
  // Ensure velocity doesn't exceed max speed after collision resolution
  pc->velocity_ = glm::clamp(pc->velocity_, -max_speed_, max_speed_);
}

void PhysicsEngine::ResolveSolidObjectCollisions(
    object::GameObject* move_obj, object::GameObject* solid_obj) {
  auto* move_tc = move_obj->GetComponent<component::TransformComponent>();
  auto* move_pc = move_obj->GetComponent<component::PhysicsComponent>();
  auto* move_cc = move_obj->GetComponent<component::ColliderComponent>();
  auto* solid_cc = solid_obj->GetComponent<component::ColliderComponent>();

  auto move_aabb = move_cc->GetWorldAABB();
  auto solid_aabb = solid_cc->GetWorldAABB();

  auto move_center = move_aabb.position + move_aabb.size / 2.0f;
  auto solid_center = solid_aabb.position + solid_aabb.size / 2.0f;
  auto overlap = glm::vec2(move_aabb.size / 2.0f + solid_aabb.size / 2.0f) -
                 glm::abs(move_center - solid_center);
  if (overlap.x < 0.1f && overlap.y < 0.1f) return;

  if (overlap.x < overlap.y) {
    if (move_center.x < solid_center.x) {
      move_tc->Translate(glm::vec2(-overlap.x, 0.0f));
      if (move_pc->velocity_.x > 0.0f) {
        move_pc->velocity_.x = 0.0f;
        move_pc->SetCollidedRight();
      }
    } else {
      move_tc->Translate(glm::vec2(overlap.x, 0.0f));
      if (move_pc->velocity_.x < 0.0f) {
        move_pc->velocity_.x = 0.0f;
        move_pc->SetCollidedLeft();
      }
    }
  } else {
    if (move_center.y < solid_center.y) {
      move_tc->Translate(glm::vec2(0.0f, -overlap.y));
      if (move_pc->velocity_.y > 0.0f) {
        move_pc->velocity_.y = 0.0f;
        move_pc->SetCollidedBelow();
      }
    } else {
      move_tc->Translate(glm::vec2(0.0f, overlap.y));
      if (move_pc->velocity_.y < 0.0f) {
        move_pc->velocity_.y = 0.0f;
        move_pc->SetCollidedAbove();
      }
    }
  }
}

float PhysicsEngine::GetTileHeightAtWidth(float width, component::TileType type,
                                          glm::vec2 tile_size) {
  auto rel_x = glm::clamp(width / tile_size.x, 0.0f, 1.0f);
  switch (type) {
    case component::TileType::kSlope0_1:
      return rel_x * tile_size.y;
    case component::TileType::kSlope0_2:
      return rel_x * tile_size.y * 0.5f;
    case component::TileType::kSlope2_1:
      return rel_x * tile_size.y * 0.5f + tile_size.y * 0.5f;
    case component::TileType::kSlope1_0:
      return (1.0f - rel_x) * tile_size.y;
    case component::TileType::kSlope2_0:
      return (1.0f - rel_x) * tile_size.y * 0.5f;
    case component::TileType::kSlope1_2:
      return (1.0f - rel_x) * tile_size.y * 0.5f + tile_size.y * 0.5f;
    default:
      return 0.0f;
  }
}

void PhysicsEngine::ApplyWorldBounds(component::PhysicsComponent* pc) {
  if (!pc || !world_bounds_) return;

  auto* obj = pc->GetOwner();
  auto* cc = obj->GetComponent<component::ColliderComponent>();
  auto* tc = obj->GetComponent<component::TransformComponent>();
  auto world_aabb = cc->GetWorldAABB();
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
  tc->Translate(obj_pos - world_aabb.position);
}

}  // namespace engine::physics
