// Copyright Sylar129

#pragma once

#include "engine/ecs/systems.h"
#include "game/ecs/components.h"

namespace engine::input {
class InputManager;
}

namespace game::ecs {

class PlayerSystem final {
 public:
  void HandleInput(entt::registry& registry,
                   const engine::input::InputManager& input_manager) const;
  void Update(entt::registry& registry, float delta_time) const;
  bool ApplyDamage(entt::registry& registry, entt::entity player_entity,
                   int damage) const;

 private:
  void EnterState(entt::registry& registry, entt::entity entity,
                  PlayerComponent& player, engine::ecs::PhysicsComponent& physics,
                  engine::ecs::SpriteComponent& sprite,
                  engine::ecs::AnimationComponent& animation,
                  engine::ecs::ColliderComponent* collider,
                  PlayerState new_state) const;
  bool IsOnGround(const PlayerComponent& player,
                  const engine::ecs::PhysicsComponent& physics) const;
  bool CanClimb(const engine::ecs::PhysicsComponent& physics,
                const PlayerInputComponent& input) const;
  void PlayAnimation(engine::ecs::AnimationComponent& animation,
                     const std::string& animation_name) const;
};

class AiSystem final {
 public:
  void Update(entt::registry& registry, float delta_time) const;

 private:
  void PlayAnimation(engine::ecs::AnimationComponent& animation,
                     const std::string& animation_name) const;
};

class GameRuleSystem final {
 public:
  RuleEvents Update(entt::registry& registry, entt::entity player_entity,
                    const std::vector<engine::ecs::CollisionPair>& collisions,
                    const std::vector<engine::ecs::TileTriggerEvent>&
                        tile_triggers) const;
};

}  // namespace game::ecs