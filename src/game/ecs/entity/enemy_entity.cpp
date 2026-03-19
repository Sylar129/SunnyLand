// Copyright Sylar129

#include "game/ecs/entity/enemy_entity.h"

#include "engine/ecs/components.h"
#include "game/ecs/components.h"

namespace game::ecs::entity {

void ConfigureEnemyEntity(entt::registry& registry, entt::entity entity) {
  if (!registry.all_of<engine::ecs::NameComponent, engine::ecs::TransformComponent>(
          entity)) {
    return;
  }

  const auto& name = registry.get<engine::ecs::NameComponent>(entity).name;
  const auto& transform = registry.get<engine::ecs::TransformComponent>(entity);

  game::ecs::AiComponent ai;
  if (name == "eagle") {
    ai.behavior = game::ecs::AiBehaviorType::kUpDown;
    ai.min_bound = transform.position.y - 80.0f;
    ai.max_bound = transform.position.y;
  } else if (name == "frog") {
    ai.behavior = game::ecs::AiBehaviorType::kJump;
    ai.min_bound = transform.position.x - 100.0f;
    ai.max_bound = transform.position.x - 10.0f;
  } else if (name == "opossum") {
    ai.behavior = game::ecs::AiBehaviorType::kPatrol;
    ai.min_bound = transform.position.x - 200.0f;
    ai.max_bound = transform.position.x;
  } else {
    return;
  }

  registry.emplace_or_replace<game::ecs::AiComponent>(entity, ai);
}

}  // namespace game::ecs::entity