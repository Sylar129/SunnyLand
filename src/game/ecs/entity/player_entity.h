// Copyright Sylar129

#pragma once

#include "entt/entt.hpp"

namespace game::ecs::entity {

void ConfigurePlayerEntity(entt::registry& registry, entt::entity player_entity,
                           int current_health, int max_health);

}  // namespace game::ecs::entity