// Copyright Sylar129

#include "engine/physics/physics_engine.h"

#include <set>

#include "engine/component/collider_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/tilelayer_component.h"
#include "engine/component/transform_component.h"
#include "engine/physics/collision.h"
#include "engine/utils/assert.h"
#include "glm/common.hpp"
#include "log.h"

namespace engine::physics {

void PhysicsEngine::RegisterComponent(
    engine::component::PhysicsComponent* component) {
  ENGINE_ASSERT(component, "PhysicsComponent is null.");
  components_.push_back(component);
  ENGINE_TRACE("Register PhysicsComponent complete.");
}

void PhysicsEngine::UnregisterComponent(
    engine::component::PhysicsComponent* component) {
  std::erase(components_, component);
  ENGINE_TRACE("Unregister PhysicsComponent complete.");
}

void PhysicsEngine::RegisterCollisionLayer(
    engine::component::TileLayerComponent* layer) {
  layer->setPhysicsEngine(this);
  collision_tile_layers_.push_back(layer);
  ENGINE_TRACE("Register collision tile layer complete.");
}

void PhysicsEngine::UnregisterCollisionLayer(
    engine::component::TileLayerComponent* layer) {
  std::erase(collision_tile_layers_, layer);
  ENGINE_TRACE("Unregister collision tile layer complete.");
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

  checkTileTriggers();
}

void PhysicsEngine::CheckObjectCollisions() {
  for (size_t i = 0; i < components_.size(); ++i) {
    auto* pc_a = components_[i];
    if (!pc_a->IsEnabled()) continue;
    auto* obj_a = pc_a->GetOwner();
    if (!obj_a) continue;
    auto* cc_a = obj_a->GetComponent<engine::component::ColliderComponent>();
    if (!cc_a || !cc_a->IsActive()) continue;

    for (size_t j = i + 1; j < components_.size(); ++j) {
      auto* pc_b = components_[j];
      if (!pc_b->IsEnabled()) continue;
      auto* obj_b = pc_b->GetOwner();
      if (!obj_b) continue;
      auto* cc_b = obj_b->GetComponent<engine::component::ColliderComponent>();
      if (!cc_b || !cc_b->IsActive()) continue;

      if (collision::CheckCollision(*cc_a, *cc_b)) {
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

void PhysicsEngine::checkTileTriggers() {
  for (auto* pc : components_) {
    if (!pc || !pc->IsEnabled()) continue;  // 检查组件是否有效和启用
    auto* obj = pc->GetOwner();
    if (!obj) continue;
    auto* cc = obj->GetComponent<engine::component::ColliderComponent>();
    if (!cc || !cc->IsActive() || cc->IsTrigger())
      continue;  // 如果游戏对象本就是触发器，则不需要检查瓦片触发事件

    // 获取物体的世界AABB
    auto world_aabb = cc->GetWorldAABB();

    // 使用 set
    // 来跟踪循环遍历中已经触发过的瓦片类型，防止重复添加（例如，玩家同时踩到两个尖刺，只需要受到一次伤害）
    std::set<engine::component::TileType> triggers_set;

    // 遍历所有注册的碰撞瓦片层分别进行检测
    for (auto* layer : collision_tile_layers_) {
      if (!layer) continue;
      auto tile_size = layer->GetTileSize();
      constexpr float tolerance =
          1.0f;  // 检查右边缘和下边缘时，需要减1像素，否则会检查到下一行/列的瓦片
      // 获取瓦片坐标范围
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

      // 遍历瓦片坐标范围进行检测
      for (int x = start_x; x < end_x; ++x) {
        for (int y = start_y; y < end_y; ++y) {
          auto tile_type = layer->GetTileTypeAt({x, y});
          // 未来可以添加更多触发器类型的瓦片，目前只有 HAZARD 类型
          if (tile_type == engine::component::TileType::HAZARD) {
            triggers_set.insert(
                tile_type);  // 记录触发事件，set 保证每个瓦片类型只记录一次
          }
        }
      }
      // 遍历触发事件集合，添加到 tile_trigger_events_ 中
      for (const auto& type : triggers_set) {
        tile_trigger_events_.emplace_back(obj, type);
        ENGINE_TRACE(
            "tile_trigger_events_中 添加了 GameObject {} 和瓦片触发类型: {}",
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
void PhysicsEngine::ResolveTileCollisions(
    engine::component::PhysicsComponent* pc, float delta_time) {
  // Validate that the physics component has an owner object
  auto* obj = pc->GetOwner();
  if (!obj) return;

  // Get required components: Transform for position updates, Collider for
  // bounds
  auto* tc = obj->GetComponent<engine::component::TransformComponent>();
  auto* cc = obj->GetComponent<engine::component::ColliderComponent>();

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

      if (tile_type_top == engine::component::TileType::SOLID ||
          tile_type_bottom == engine::component::TileType::SOLID) {
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

      if (tile_type_top == engine::component::TileType::SOLID ||
          tile_type_bottom == engine::component::TileType::SOLID) {
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

      if (tile_type_left == engine::component::TileType::SOLID ||
          tile_type_right == engine::component::TileType::SOLID ||
          tile_type_left == engine::component::TileType::UNISOLID ||
          tile_type_right == engine::component::TileType::UNISOLID) {
        new_obj_pos.y = tile_y * tile_size.y - obj_size.y;
        pc->velocity_.y = 0.0f;
        pc->SetCollidedBelow();
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
      if (tile_type_left == engine::component::TileType::SOLID ||
          tile_type_right == engine::component::TileType::SOLID) {
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
    engine::object::GameObject* move_obj,
    engine::object::GameObject* solid_obj) {
  auto* move_tc =
      move_obj->GetComponent<engine::component::TransformComponent>();
  auto* move_pc = move_obj->GetComponent<engine::component::PhysicsComponent>();
  auto* move_cc =
      move_obj->GetComponent<engine::component::ColliderComponent>();
  auto* solid_cc =
      solid_obj->GetComponent<engine::component::ColliderComponent>();

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

float PhysicsEngine::GetTileHeightAtWidth(float width,
                                          engine::component::TileType type,
                                          glm::vec2 tile_size) {
  auto rel_x = glm::clamp(width / tile_size.x, 0.0f, 1.0f);
  switch (type) {
    case engine::component::TileType::SLOPE_0_1:
      return rel_x * tile_size.y;
    case engine::component::TileType::SLOPE_0_2:
      return rel_x * tile_size.y * 0.5f;
    case engine::component::TileType::SLOPE_2_1:
      return rel_x * tile_size.y * 0.5f + tile_size.y * 0.5f;
    case engine::component::TileType::SLOPE_1_0:
      return (1.0f - rel_x) * tile_size.y;
    case engine::component::TileType::SLOPE_2_0:
      return (1.0f - rel_x) * tile_size.y * 0.5f;
    case engine::component::TileType::SLOPE_1_2:
      return (1.0f - rel_x) * tile_size.y * 0.5f + tile_size.y * 0.5f;
    default:
      return 0.0f;
  }
}

void PhysicsEngine::ApplyWorldBounds(engine::component::PhysicsComponent* pc) {
  if (!pc || !world_bounds_) return;

  auto* obj = pc->GetOwner();
  auto* cc = obj->GetComponent<engine::component::ColliderComponent>();
  auto* tc = obj->GetComponent<engine::component::TransformComponent>();
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
