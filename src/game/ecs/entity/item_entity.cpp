// Copyright Sylar129

#include "game/ecs/entity/item_entity.h"

#include "engine/ecs/components.h"

namespace game::ecs::entity {

void ConfigureItemEntity(entt::registry& registry, entt::entity entity) {
  const auto* tag = registry.try_get<engine::ecs::TagComponent>(entity);
  auto* animation = registry.try_get<engine::ecs::AnimationComponent>(entity);
  if (tag == nullptr || tag->tag != "item" || animation == nullptr) {
    return;
  }

  animation->current_animation = "idle";
  animation->animation_timer = 0.0f;
  animation->is_playing = true;
}

}  // namespace game::ecs::entity