// Copyright Sylar129

#include "game/ecs/entity/player_entity.h"

#include "engine/ecs/components.h"
#include "game/ecs/components.h"

namespace game::ecs::entity {

void ConfigurePlayerEntity(entt::registry& registry, entt::entity player_entity,
                           int current_health, int max_health) {
  registry.emplace_or_replace<game::ecs::PlayerComponent>(player_entity);
  registry.emplace_or_replace<game::ecs::PlayerInputComponent>(player_entity);
  registry.emplace_or_replace<engine::ecs::CameraTargetComponent>(
      player_entity);

  if (registry.all_of<engine::ecs::HealthComponent>(player_entity)) {
    auto& health = registry.get<engine::ecs::HealthComponent>(player_entity);
    health.current_health = current_health;
    health.max_health = max_health;
  }
}

}  // namespace game::ecs::entity