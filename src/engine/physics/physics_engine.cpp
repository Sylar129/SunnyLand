// Copyright Sylar129

#include "engine/physics/physics_engine.h"

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

void PhysicsEngine::registerCollisionLayer(
    engine::component::TileLayerComponent* layer) {
  layer->setPhysicsEngine(this);  // 设置物理引擎指针
  collision_tile_layers_.push_back(layer);
  ENGINE_TRACE("碰撞瓦片图层注册完成。");
}

void PhysicsEngine::unregisterCollisionLayer(
    engine::component::TileLayerComponent* layer) {
  auto it = std::remove(collision_tile_layers_.begin(),
                        collision_tile_layers_.end(), layer);
  collision_tile_layers_.erase(it, collision_tile_layers_.end());
  ENGINE_TRACE("碰撞瓦片图层注销完成。");
}

void PhysicsEngine::Update(float delta_time) {
  collision_pairs_.clear();

  for (auto* pc : components_) {
    if (!pc->IsEnabled()) {
      continue;
    }

    if (pc->IsUseGravity()) {
      pc->AddForce(gravity_ * pc->GetMass());
    }

    pc->velocity_ += (pc->GetForce() / pc->GetMass()) * delta_time;
    pc->velocity_ = glm::clamp(pc->velocity_, -max_speed_, max_speed_);
    pc->ClearForce();

    resolveTileCollisions(pc, delta_time);
  }

  CheckObjectCollisions();
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
        collision_pairs_.emplace_back(obj_a, obj_b);
      }
    }
  }
}

void PhysicsEngine::resolveTileCollisions(
    engine::component::PhysicsComponent* pc, float delta_time) {
  // 检查组件是否有效
  auto* obj = pc->GetOwner();
  if (!obj) return;
  auto* tc = obj->GetComponent<engine::component::TransformComponent>();
  auto* cc = obj->GetComponent<engine::component::ColliderComponent>();
  if (!tc || !cc || !cc->IsActive() || cc->IsTrigger()) return;
  auto world_aabb = cc->GetWorldAABB();  // 使用最小包围盒进行碰撞检测（简化）
  auto obj_pos = world_aabb.position;
  auto obj_size = world_aabb.size;
  if (world_aabb.size.x <= 0.0f || world_aabb.size.y <= 0.0f) return;
  // -- 检查结束, 正式开始处理 --

  auto tolerance =
      1.0f;  // 检查右边缘和下边缘时，需要减1像素，否则会检查到下一行/列的瓦片
  auto ds = pc->velocity_ * delta_time;  // 计算物体在delta_time内的位移
  auto new_obj_pos = obj_pos + ds;       // 计算物体在delta_time后的新位置

  // 遍历所有注册的碰撞瓦片层
  for (auto* layer : collision_tile_layers_) {
    if (!layer) continue;
    auto tile_size = layer->GetTileSize();
    // 轴分离碰撞检测：先检查X方向是否有碰撞 (y方向使用初始值obj_pos.y)
    if (ds.x > 0.0f) {
      // 检查右侧碰撞，需要分别测试右上和右下角
      auto right_top_x = new_obj_pos.x + obj_size.x;
      auto tile_x = static_cast<int>(
          floor(right_top_x / tile_size.x));  // 获取x方向瓦片坐标
      // y方向坐标有两个，右上和右下
      auto tile_y = static_cast<int>(floor(obj_pos.y / tile_size.y));
      auto tile_type_top =
          layer->GetTileTypeAt({tile_x, tile_y});  // 右上角瓦片类型
      auto tile_y_bottom = static_cast<int>(
          floor((obj_pos.y + obj_size.y - tolerance) / tile_size.y));
      auto tile_type_bottom =
          layer->GetTileTypeAt({tile_x, tile_y_bottom});  // 右下角瓦片类型

      if (tile_type_top == engine::component::TileType::SOLID ||
          tile_type_bottom == engine::component::TileType::SOLID) {
        // 撞墙了！速度归零，x方向移动到贴着墙的位置
        new_obj_pos.x = tile_x * layer->GetTileSize().x - obj_size.x;
        pc->velocity_.x = 0.0f;
      }
    } else if (ds.x < 0.0f) {
      // 检查左侧碰撞，需要分别测试左上和左下角
      auto left_top_x = new_obj_pos.x;
      auto tile_x = static_cast<int>(
          floor(left_top_x / tile_size.x));  // 获取x方向瓦片坐标
      // y方向坐标有两个，左上和左下
      auto tile_y = static_cast<int>(floor(obj_pos.y / tile_size.y));
      auto tile_type_top =
          layer->GetTileTypeAt({tile_x, tile_y});  // 左上角瓦片类型
      auto tile_y_bottom = static_cast<int>(
          floor((obj_pos.y + obj_size.y - tolerance) / tile_size.y));
      auto tile_type_bottom =
          layer->GetTileTypeAt({tile_x, tile_y_bottom});  // 左下角瓦片类型

      if (tile_type_top == engine::component::TileType::SOLID ||
          tile_type_bottom == engine::component::TileType::SOLID) {
        // 撞墙了！速度归零，x方向移动到贴着墙的位置
        new_obj_pos.x = (tile_x + 1) * layer->GetTileSize().x;
        pc->velocity_.x = 0.0f;
      }
    }
    // 轴分离碰撞检测：再检查Y方向是否有碰撞 (x方向使用初始值obj_pos.x)
    if (ds.y > 0.0f) {
      // 检查底部碰撞，需要分别测试左下和右下角
      auto bottom_left_y = new_obj_pos.y + obj_size.y;
      auto tile_y = static_cast<int>(floor(bottom_left_y / tile_size.y));

      auto tile_x = static_cast<int>(floor(obj_pos.x / tile_size.x));
      auto tile_type_left =
          layer->GetTileTypeAt({tile_x, tile_y});  // 左下角瓦片类型
      auto tile_x_right = static_cast<int>(
          floor((obj_pos.x + obj_size.x - tolerance) / tile_size.x));
      auto tile_type_right =
          layer->GetTileTypeAt({tile_x_right, tile_y});  // 右下角瓦片类型

      if (tile_type_left == engine::component::TileType::SOLID ||
          tile_type_right == engine::component::TileType::SOLID) {
        // 到达地面！速度归零，y方向移动到贴着地面的位置
        new_obj_pos.y = tile_y * layer->GetTileSize().y - obj_size.y;
        pc->velocity_.y = 0.0f;
      }
    } else if (ds.y < 0.0f) {
      // 检查顶部碰撞，需要分别测试左上和右上角
      auto top_left_y = new_obj_pos.y;
      auto tile_y = static_cast<int>(floor(top_left_y / tile_size.y));

      auto tile_x = static_cast<int>(floor(obj_pos.x / tile_size.x));
      auto tile_type_left =
          layer->GetTileTypeAt({tile_x, tile_y});  // 左上角瓦片类型
      auto tile_x_right = static_cast<int>(
          floor((obj_pos.x + obj_size.x - tolerance) / tile_size.x));
      auto tile_type_right =
          layer->GetTileTypeAt({tile_x_right, tile_y});  // 右上角瓦片类型

      if (tile_type_left == engine::component::TileType::SOLID ||
          tile_type_right == engine::component::TileType::SOLID) {
        // 撞到天花板！速度归零，y方向移动到贴着天花板的位置
        new_obj_pos.y = (tile_y + 1) * layer->GetTileSize().y;
        pc->velocity_.y = 0.0f;
      }
    }
  }
  // 更新物体位置，并限制最大速度
  tc->SetPosition(new_obj_pos);
  pc->velocity_ = glm::clamp(pc->velocity_, -max_speed_, max_speed_);
}

}  // namespace engine::physics
