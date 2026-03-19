// Copyright Sylar129

#include "engine/ecs/systems.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <vector>

#include "engine/core/context.h"
#include "engine/render/renderer.h"
#include "engine/resource/resource_manager.h"
#include "glm/common.hpp"

namespace engine::ecs {
namespace {

int GetRenderOrder(const entt::registry& registry, entt::entity entity) {
  if (const auto* render_order = registry.try_get<RenderOrderComponent>(
          entity)) {
    return render_order->value;
  }
  return 0;
}

TileType GetTileTypeAt(const TileLayerComponent& layer, const glm::ivec2& pos) {
  if (pos.x < 0 || pos.y < 0 || pos.x >= layer.map_size.x ||
      pos.y >= layer.map_size.y) {
    return TileType::kEmpty;
  }
  const size_t index =
      static_cast<size_t>(pos.y * layer.map_size.x + pos.x);
  if (index >= layer.tiles.size()) {
    return TileType::kEmpty;
  }
  return layer.tiles[index].type;
}

const std::string& GetTag(const entt::registry& registry, entt::entity entity) {
  static const std::string kEmptyTag;
  if (const auto* tag = registry.try_get<TagComponent>(entity)) {
    return tag->tag;
  }
  return kEmptyTag;
}

glm::vec2 GetSpriteSize(const render::Sprite& sprite,
                        resource::ResourceManager& resource_manager) {
  if (const auto& source_rect = sprite.GetSourceRect(); source_rect.has_value()) {
    return {source_rect->w, source_rect->h};
  }
  return resource_manager.GetTextureSize(sprite.GetTextureId());
}

bool AreOverlapping(const utils::Rect& first, const utils::Rect& second) {
  const bool overlap_x =
      first.position.x < second.position.x + second.size.x &&
      first.position.x + first.size.x > second.position.x;
  const bool overlap_y =
      first.position.y < second.position.y + second.size.y &&
      first.position.y + first.size.y > second.position.y;
  return overlap_x && overlap_y;
}

}  // namespace

glm::vec2 GetAlignmentOffset(utils::Alignment alignment, const glm::vec2& size,
                             const glm::vec2& scale) {
  switch (alignment) {
    case utils::Alignment::kTopCenter:
      return glm::vec2{-size.x / 2.0f, 0.0f} * scale;
    case utils::Alignment::kTopRight:
      return glm::vec2{-size.x, 0.0f} * scale;
    case utils::Alignment::kCenterLeft:
      return glm::vec2{0.0f, -size.y / 2.0f} * scale;
    case utils::Alignment::kCenter:
      return glm::vec2{-size.x / 2.0f, -size.y / 2.0f} * scale;
    case utils::Alignment::kCenterRight:
      return glm::vec2{-size.x, -size.y / 2.0f} * scale;
    case utils::Alignment::kBottomLeft:
      return glm::vec2{0.0f, -size.y} * scale;
    case utils::Alignment::kBottomCenter:
      return glm::vec2{-size.x / 2.0f, -size.y} * scale;
    case utils::Alignment::kBottomRight:
      return glm::vec2{-size.x, -size.y} * scale;
    case utils::Alignment::kTopLeft:
    case utils::Alignment::kNone:
    default:
      return {0.0f, 0.0f};
  }
}

utils::Rect GetWorldAabb(const TransformComponent& transform,
                         const ColliderComponent& collider) {
  const glm::vec2 alignment_offset =
      GetAlignmentOffset(collider.alignment, collider.size, transform.scale);
  const glm::vec2 position = transform.position + alignment_offset +
                             collider.offset;
  return {position, collider.size * transform.scale};
}

bool HasCollisionFlag(const PhysicsComponent& physics,
                      PhysicsComponent::CollisionFlag flag) {
  return (physics.collision_flags & flag) != 0;
}

void SetCollisionFlag(PhysicsComponent& physics,
                      PhysicsComponent::CollisionFlag flag) {
  physics.collision_flags |= flag;
}

void ClearCollisionFlags(PhysicsComponent& physics) { physics.collision_flags = 0; }

void DestroyPendingEntities(entt::registry& registry) {
  auto view = registry.view<PendingDestroyComponent>();
  std::vector<entt::entity> entities(view.begin(), view.end());
  for (const auto entity : entities) {
    if (registry.valid(entity)) {
      registry.destroy(entity);
    }
  }
}

void AnimationSystem::Update(entt::registry& registry, float delta_time) const {
  auto view = registry.view<AnimationComponent, SpriteComponent>();
  for (const auto entity : view) {
    auto& animation = view.get<AnimationComponent>(entity);
    auto& sprite = view.get<SpriteComponent>(entity);
    if (!animation.is_playing || animation.current_animation.empty()) {
      continue;
    }

    const auto animation_it =
        animation.animations.find(animation.current_animation);
    if (animation_it == animation.animations.end() ||
        !animation_it->second || animation_it->second->IsEmpty()) {
      continue;
    }

    animation.animation_timer += delta_time;
    const auto& current_animation = *animation_it->second;
    const auto frame = current_animation.GetFrame(animation.animation_timer);
    sprite.sprite.SetSourceRect(frame.source_rect);
    sprite.sprite_size = {frame.source_rect.w, frame.source_rect.h};

    if (!current_animation.IsLooping() &&
        animation.animation_timer >= current_animation.GetTotalDuration()) {
      animation.is_playing = false;
      if (animation.is_one_shot_removal) {
        registry.emplace_or_replace<PendingDestroyComponent>(entity);
      }
    }
  }
}

void RenderSystem::Render(entt::registry& registry,
                          core::Context& context) const {
  auto parallax_view = registry.view<ParallaxComponent, TransformComponent>();
  std::vector<entt::entity> parallax_entities(parallax_view.begin(),
                                              parallax_view.end());
  std::sort(parallax_entities.begin(), parallax_entities.end(),
            [&registry](const entt::entity lhs, const entt::entity rhs) {
              return GetRenderOrder(registry, lhs) < GetRenderOrder(registry, rhs);
            });
  for (const auto entity : parallax_entities) {
    const auto& parallax = parallax_view.get<ParallaxComponent>(entity);
    const auto& transform = parallax_view.get<TransformComponent>(entity);
    if (parallax.hidden) {
      continue;
    }
    context.GetRenderer().DrawParallax(context.GetCamera(), parallax.sprite,
                                       transform.position,
                                       parallax.scroll_factor, parallax.repeat,
                                       transform.scale);
  }

  auto tile_view = registry.view<TileLayerComponent>();
  std::vector<entt::entity> tile_entities(tile_view.begin(), tile_view.end());
  std::sort(tile_entities.begin(), tile_entities.end(),
            [&registry](const entt::entity lhs, const entt::entity rhs) {
              return GetRenderOrder(registry, lhs) < GetRenderOrder(registry, rhs);
            });
  for (const auto entity : tile_entities) {
    const auto& layer = tile_view.get<TileLayerComponent>(entity);
    if (layer.hidden) {
      continue;
    }
    for (int y = 0; y < layer.map_size.y; ++y) {
      for (int x = 0; x < layer.map_size.x; ++x) {
        const size_t index = static_cast<size_t>(y * layer.map_size.x + x);
        if (index >= layer.tiles.size()) {
          continue;
        }
        const auto& tile = layer.tiles[index];
        if (tile.type == TileType::kEmpty || tile.sprite.GetTextureId().empty()) {
          continue;
        }
        const glm::vec2 tile_position =
            layer.offset +
            glm::vec2(static_cast<float>(x * layer.tile_size.x),
                      static_cast<float>(y * layer.tile_size.y));
        context.GetRenderer().DrawSprite(context.GetCamera(), tile.sprite,
                                         tile_position);
      }
    }
  }

  auto sprite_view = registry.view<SpriteComponent, TransformComponent>();
  std::vector<entt::entity> sprite_entities(sprite_view.begin(),
                                            sprite_view.end());
  std::sort(sprite_entities.begin(), sprite_entities.end(),
            [&registry](const entt::entity lhs, const entt::entity rhs) {
              return GetRenderOrder(registry, lhs) < GetRenderOrder(registry, rhs);
            });
  for (const auto entity : sprite_entities) {
    auto& sprite = sprite_view.get<SpriteComponent>(entity);
    const auto& transform = sprite_view.get<TransformComponent>(entity);
    if (sprite.hidden) {
      continue;
    }
    sprite.sprite_size =
        GetSpriteSize(sprite.sprite, context.GetResourceManager());
    sprite.offset =
        GetAlignmentOffset(sprite.alignment, sprite.sprite_size, transform.scale);
    context.GetRenderer().DrawSprite(context.GetCamera(), sprite.sprite,
                                     transform.position + sprite.offset,
                                     transform.scale,
                                     static_cast<double>(transform.rotation));
  }
}

void PhysicsSystem::Update(entt::registry& registry, float delta_time) {
  collision_pairs_.clear();
  tile_trigger_events_.clear();

  auto physics_view = registry.view<PhysicsComponent, TransformComponent>();
  for (const auto entity : physics_view) {
    auto& physics = physics_view.get<PhysicsComponent>(entity);
    auto& transform = physics_view.get<TransformComponent>(entity);
    if (!physics.enabled) {
      continue;
    }

    ClearCollisionFlags(physics);
    if (physics.use_gravity) {
      physics.force += gravity_ * physics.mass;
    }

    const float mass = physics.mass > 0.0f ? physics.mass : 1.0f;
    physics.velocity += (physics.force / mass) * delta_time;
    physics.velocity = glm::clamp(physics.velocity, -max_speed_, max_speed_);
    physics.force = {0.0f, 0.0f};

    ResolveTileCollisions(registry, entity, physics, transform, delta_time);
    ApplyWorldBounds(registry, entity, physics, transform);
  }

  CheckObjectCollisions(registry);
  CheckTileTriggers(registry);
}

void PhysicsSystem::CheckObjectCollisions(entt::registry& registry) {
  auto view =
      registry.view<PhysicsComponent, TransformComponent, ColliderComponent>();
  std::vector<entt::entity> entities(view.begin(), view.end());
  for (size_t i = 0; i < entities.size(); ++i) {
    const auto first = entities[i];
    auto& first_physics = view.get<PhysicsComponent>(first);
    auto& first_transform = view.get<TransformComponent>(first);
    auto& first_collider = view.get<ColliderComponent>(first);
    if (!first_physics.enabled || !first_collider.is_active) {
      continue;
    }
    const auto first_aabb = GetWorldAabb(first_transform, first_collider);

    for (size_t j = i + 1; j < entities.size(); ++j) {
      const auto second = entities[j];
      auto& second_physics = view.get<PhysicsComponent>(second);
      auto& second_transform = view.get<TransformComponent>(second);
      auto& second_collider = view.get<ColliderComponent>(second);
      if (!second_physics.enabled || !second_collider.is_active) {
        continue;
      }
      const auto second_aabb = GetWorldAabb(second_transform, second_collider);
      if (!AreOverlapping(first_aabb, second_aabb)) {
        continue;
      }

      const bool first_is_solid = GetTag(registry, first) == "solid";
      const bool second_is_solid = GetTag(registry, second) == "solid";
      if (!first_is_solid && second_is_solid) {
        ResolveSolidObjectCollisions(registry, first, second);
      } else if (first_is_solid && !second_is_solid) {
        ResolveSolidObjectCollisions(registry, second, first);
      } else {
        collision_pairs_.push_back({first, second});
      }
    }
  }
}

void PhysicsSystem::CheckTileTriggers(entt::registry& registry) {
  auto physics_view =
      registry.view<PhysicsComponent, TransformComponent, ColliderComponent>();
  auto tile_view = registry.view<TileLayerComponent>();
  for (const auto entity : physics_view) {
    auto& physics = physics_view.get<PhysicsComponent>(entity);
    const auto& transform = physics_view.get<TransformComponent>(entity);
    const auto& collider = physics_view.get<ColliderComponent>(entity);
    if (!physics.enabled || !collider.is_active || collider.is_trigger) {
      continue;
    }
    const auto world_aabb = GetWorldAabb(transform, collider);
    std::set<TileType> trigger_types;
    for (const auto layer_entity : tile_view) {
      const auto& layer = tile_view.get<TileLayerComponent>(layer_entity);
        const float tile_width = static_cast<float>(layer.tile_size.x);
        const float tile_height = static_cast<float>(layer.tile_size.y);
      constexpr float kTolerance = 1.0f;
      const auto start_x = static_cast<int>(
          std::floor(world_aabb.position.x / tile_width));
      const auto end_x = static_cast<int>(std::ceil(
          (world_aabb.position.x + world_aabb.size.x - kTolerance) /
          tile_width));
      const auto start_y = static_cast<int>(
          std::floor(world_aabb.position.y / tile_height));
      const auto end_y = static_cast<int>(std::ceil(
          (world_aabb.position.y + world_aabb.size.y - kTolerance) /
          tile_height));
      for (int x = start_x; x < end_x; ++x) {
        for (int y = start_y; y < end_y; ++y) {
          const auto tile_type = GetTileTypeAt(layer, {x, y});
          if (tile_type == TileType::kHazard) {
            trigger_types.insert(tile_type);
          } else if (tile_type == TileType::kLadder) {
            SetCollisionFlag(physics, PhysicsComponent::kCollidedLadder);
          }
        }
      }
    }

    for (const auto tile_type : trigger_types) {
      tile_trigger_events_.push_back({entity, tile_type});
    }
  }
}

void PhysicsSystem::ResolveTileCollisions(entt::registry& registry,
                                          entt::entity entity,
                                          PhysicsComponent& physics,
                                          TransformComponent& transform,
                                          float delta_time) {
  const auto* collider = registry.try_get<ColliderComponent>(entity);
  if (collider == nullptr || collider->is_trigger) {
    transform.position += physics.velocity * delta_time;
    return;
  }

  const auto world_aabb = GetWorldAabb(transform, *collider);
  if (!collider->is_active || world_aabb.size.x <= 0.0f ||
      world_aabb.size.y <= 0.0f) {
    transform.position += physics.velocity * delta_time;
    return;
  }

  constexpr float kTolerance = 1.0f;
  const glm::vec2 displacement = physics.velocity * delta_time;
  glm::vec2 new_position = world_aabb.position + displacement;
  const auto tile_view = registry.view<TileLayerComponent>();
  for (const auto layer_entity : tile_view) {
    const auto& layer = tile_view.get<TileLayerComponent>(layer_entity);
    const glm::vec2 tile_size(static_cast<float>(layer.tile_size.x),
                  static_cast<float>(layer.tile_size.y));

    if (displacement.x > 0.0f) {
      const float right_x = new_position.x + world_aabb.size.x;
      const int tile_x = static_cast<int>(std::floor(right_x / tile_size.x));
      const int tile_y =
          static_cast<int>(std::floor(world_aabb.position.y / tile_size.y));
      const int tile_y_bottom = static_cast<int>(std::floor(
          (world_aabb.position.y + world_aabb.size.y - kTolerance) /
          tile_size.y));
      const auto tile_type_top = GetTileTypeAt(layer, {tile_x, tile_y});
      const auto tile_type_bottom =
          GetTileTypeAt(layer, {tile_x, tile_y_bottom});
      if (tile_type_top == TileType::kSolid ||
          tile_type_bottom == TileType::kSolid) {
        new_position.x = static_cast<float>(tile_x) * tile_size.x -
             world_aabb.size.x;
        physics.velocity.x = 0.0f;
        SetCollisionFlag(physics, PhysicsComponent::kCollidedRight);
      } else {
        const float width_right =
            new_position.x + world_aabb.size.x -
            static_cast<float>(tile_x) * tile_size.x;
        const float height_right =
            GetTileHeightAtWidth(width_right, tile_type_bottom, tile_size);
        if (height_right > 0.0f &&
            new_position.y > static_cast<float>(tile_y_bottom + 1) * tile_size.y -
                                 world_aabb.size.y - height_right) {
          new_position.y = static_cast<float>(tile_y_bottom + 1) * tile_size.y -
                           world_aabb.size.y - height_right;
          SetCollisionFlag(physics, PhysicsComponent::kCollidedBelow);
        }
      }
    } else if (displacement.x < 0.0f) {
      const float left_x = new_position.x;
      const int tile_x = static_cast<int>(std::floor(left_x / tile_size.x));
      const int tile_y =
          static_cast<int>(std::floor(world_aabb.position.y / tile_size.y));
      const int tile_y_bottom = static_cast<int>(std::floor(
          (world_aabb.position.y + world_aabb.size.y - kTolerance) /
          tile_size.y));
      const auto tile_type_top = GetTileTypeAt(layer, {tile_x, tile_y});
      const auto tile_type_bottom =
          GetTileTypeAt(layer, {tile_x, tile_y_bottom});
      if (tile_type_top == TileType::kSolid ||
          tile_type_bottom == TileType::kSolid) {
        new_position.x = static_cast<float>(tile_x + 1) * tile_size.x;
        physics.velocity.x = 0.0f;
        SetCollisionFlag(physics, PhysicsComponent::kCollidedLeft);
      } else {
        const float width_left =
          new_position.x - static_cast<float>(tile_x) * tile_size.x;
        const float height_left =
            GetTileHeightAtWidth(width_left, tile_type_bottom, tile_size);
        if (height_left > 0.0f &&
            new_position.y > static_cast<float>(tile_y_bottom + 1) * tile_size.y -
                                 world_aabb.size.y - height_left) {
          new_position.y = static_cast<float>(tile_y_bottom + 1) * tile_size.y -
                           world_aabb.size.y - height_left;
          SetCollisionFlag(physics, PhysicsComponent::kCollidedBelow);
        }
      }
    }

    if (displacement.y > 0.0f) {
      const float bottom_y = new_position.y + world_aabb.size.y;
      const int tile_y = static_cast<int>(std::floor(bottom_y / tile_size.y));
      const int tile_x =
          static_cast<int>(std::floor(world_aabb.position.x / tile_size.x));
      const int tile_x_right = static_cast<int>(std::floor(
          (world_aabb.position.x + world_aabb.size.x - kTolerance) /
          tile_size.x));
      const auto tile_type_left = GetTileTypeAt(layer, {tile_x, tile_y});
      const auto tile_type_right =
          GetTileTypeAt(layer, {tile_x_right, tile_y});
      if (tile_type_left == TileType::kSolid ||
          tile_type_right == TileType::kSolid ||
          tile_type_left == TileType::kUnisolid ||
          tile_type_right == TileType::kUnisolid) {
        new_position.y = static_cast<float>(tile_y) * tile_size.y -
             world_aabb.size.y;
        physics.velocity.y = 0.0f;
        SetCollisionFlag(physics, PhysicsComponent::kCollidedBelow);
      } else if (tile_type_left == TileType::kLadder &&
                 tile_type_right == TileType::kLadder) {
        const auto tile_up_left = GetTileTypeAt(layer, {tile_x, tile_y - 1});
        const auto tile_up_right =
            GetTileTypeAt(layer, {tile_x_right, tile_y - 1});
        if (tile_up_left != TileType::kLadder &&
            tile_up_right != TileType::kLadder) {
          if (physics.use_gravity) {
            SetCollisionFlag(physics, PhysicsComponent::kIsOnTopLadder);
            SetCollisionFlag(physics, PhysicsComponent::kCollidedBelow);
            new_position.y = static_cast<float>(tile_y) * tile_size.y -
                             world_aabb.size.y;
            physics.velocity.y = 0.0f;
          }
        }
      } else {
        const float width_left =
          world_aabb.position.x - static_cast<float>(tile_x) * tile_size.x;
        const float width_right = world_aabb.position.x + world_aabb.size.x -
                      static_cast<float>(tile_x_right) *
                        tile_size.x;
        const float height_left =
            GetTileHeightAtWidth(width_left, tile_type_left, tile_size);
        const float height_right =
            GetTileHeightAtWidth(width_right, tile_type_right, tile_size);
        const float height = glm::max(height_left, height_right);
        if (height > 0.0f &&
            new_position.y > static_cast<float>(tile_y + 1) * tile_size.y -
                                 world_aabb.size.y -
                                 height) {
          new_position.y =
              static_cast<float>(tile_y + 1) * tile_size.y -
              world_aabb.size.y - height;
          physics.velocity.y = 0.0f;
          SetCollisionFlag(physics, PhysicsComponent::kCollidedBelow);
        }
      }
    } else if (displacement.y < 0.0f) {
      const float top_y = new_position.y;
      const int tile_y = static_cast<int>(std::floor(top_y / tile_size.y));
      const int tile_x =
          static_cast<int>(std::floor(world_aabb.position.x / tile_size.x));
      const int tile_x_right = static_cast<int>(std::floor(
          (world_aabb.position.x + world_aabb.size.x - kTolerance) /
          tile_size.x));
      const auto tile_type_left = GetTileTypeAt(layer, {tile_x, tile_y});
      const auto tile_type_right =
          GetTileTypeAt(layer, {tile_x_right, tile_y});
      if (tile_type_left == TileType::kSolid ||
          tile_type_right == TileType::kSolid) {
        new_position.y = static_cast<float>(tile_y + 1) * tile_size.y;
        physics.velocity.y = 0.0f;
        SetCollisionFlag(physics, PhysicsComponent::kCollidedAbove);
      }
    }
  }

  transform.position += new_position - world_aabb.position;
  physics.velocity = glm::clamp(physics.velocity, -max_speed_, max_speed_);
}

void PhysicsSystem::ResolveSolidObjectCollisions(entt::registry& registry,
                                                 entt::entity moving_entity,
                                                 entt::entity solid_entity) {
  auto& moving_transform = registry.get<TransformComponent>(moving_entity);
  auto& moving_physics = registry.get<PhysicsComponent>(moving_entity);
  const auto& moving_collider = registry.get<ColliderComponent>(moving_entity);
  const auto& solid_transform = registry.get<TransformComponent>(solid_entity);
  const auto& solid_collider = registry.get<ColliderComponent>(solid_entity);

  const auto moving_aabb = GetWorldAabb(moving_transform, moving_collider);
  const auto solid_aabb = GetWorldAabb(solid_transform, solid_collider);
  const glm::vec2 moving_center = moving_aabb.position + moving_aabb.size / 2.0f;
  const glm::vec2 solid_center = solid_aabb.position + solid_aabb.size / 2.0f;
  const glm::vec2 overlap =
      glm::vec2(moving_aabb.size / 2.0f + solid_aabb.size / 2.0f) -
      glm::abs(moving_center - solid_center);
  if (overlap.x < 0.1f && overlap.y < 0.1f) {
    return;
  }

  if (overlap.x < overlap.y) {
    if (moving_center.x < solid_center.x) {
      moving_transform.position += glm::vec2(-overlap.x, 0.0f);
      if (moving_physics.velocity.x > 0.0f) {
        moving_physics.velocity.x = 0.0f;
        SetCollisionFlag(moving_physics, PhysicsComponent::kCollidedRight);
      }
    } else {
      moving_transform.position += glm::vec2(overlap.x, 0.0f);
      if (moving_physics.velocity.x < 0.0f) {
        moving_physics.velocity.x = 0.0f;
        SetCollisionFlag(moving_physics, PhysicsComponent::kCollidedLeft);
      }
    }
  } else {
    if (moving_center.y < solid_center.y) {
      moving_transform.position += glm::vec2(0.0f, -overlap.y);
      if (moving_physics.velocity.y > 0.0f) {
        moving_physics.velocity.y = 0.0f;
        SetCollisionFlag(moving_physics, PhysicsComponent::kCollidedBelow);
      }
    } else {
      moving_transform.position += glm::vec2(0.0f, overlap.y);
      if (moving_physics.velocity.y < 0.0f) {
        moving_physics.velocity.y = 0.0f;
        SetCollisionFlag(moving_physics, PhysicsComponent::kCollidedAbove);
      }
    }
  }
}

float PhysicsSystem::GetTileHeightAtWidth(float width, TileType type,
                                          const glm::vec2& tile_size) const {
  const float rel_x = glm::clamp(width / tile_size.x, 0.0f, 1.0f);
  switch (type) {
    case TileType::kSlope0_1:
      return rel_x * tile_size.y;
    case TileType::kSlope0_2:
      return rel_x * tile_size.y * 0.5f;
    case TileType::kSlope2_1:
      return rel_x * tile_size.y * 0.5f + tile_size.y * 0.5f;
    case TileType::kSlope1_0:
      return (1.0f - rel_x) * tile_size.y;
    case TileType::kSlope2_0:
      return (1.0f - rel_x) * tile_size.y * 0.5f;
    case TileType::kSlope1_2:
      return (1.0f - rel_x) * tile_size.y * 0.5f + tile_size.y * 0.5f;
    default:
      return 0.0f;
  }
}

void PhysicsSystem::ApplyWorldBounds(entt::registry& registry,
                                     entt::entity entity,
                                     PhysicsComponent& physics,
                                     TransformComponent& transform) const {
  if (!world_bounds_.has_value() || !registry.all_of<ColliderComponent>(entity)) {
    return;
  }
  const auto& collider = registry.get<ColliderComponent>(entity);
  const auto world_aabb = GetWorldAabb(transform, collider);
  glm::vec2 position = world_aabb.position;
  if (position.x < world_bounds_->position.x) {
    physics.velocity.x = 0.0f;
    position.x = world_bounds_->position.x;
    SetCollisionFlag(physics, PhysicsComponent::kCollidedLeft);
  }
  if (position.y < world_bounds_->position.y) {
    physics.velocity.y = 0.0f;
    position.y = world_bounds_->position.y;
    SetCollisionFlag(physics, PhysicsComponent::kCollidedAbove);
  }
  if (position.x + world_aabb.size.x >
      world_bounds_->position.x + world_bounds_->size.x) {
    physics.velocity.x = 0.0f;
    position.x = world_bounds_->position.x + world_bounds_->size.x -
                 world_aabb.size.x;
    SetCollisionFlag(physics, PhysicsComponent::kCollidedRight);
  }
  transform.position += position - world_aabb.position;
}

}  // namespace engine::ecs