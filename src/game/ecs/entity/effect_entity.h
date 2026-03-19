// Copyright Sylar129

#pragma once

#include <string>

#include "entt/entt.hpp"
#include "glm/vec2.hpp"

namespace engine::resource {
class ResourceManager;
}

namespace game::ecs::entity {

entt::entity CreateEffectEntity(entt::registry& registry,
                                engine::resource::ResourceManager& resource_manager,
                                const glm::vec2& center_pos,
                                const std::string& tag);

}  // namespace game::ecs::entity