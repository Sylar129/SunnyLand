// Copyright Sylar129

#include "game/ecs/systems.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>

#include "engine/input/input_manager.h"
#include "glm/common.hpp"

namespace game::ecs {
namespace {

const std::string& GetName(const entt::registry& registry, entt::entity entity) {
  static const std::string kEmptyName;
  if (const auto* name = registry.try_get<engine::ecs::NameComponent>(entity)) {
    return name->name;
  }
  return kEmptyName;
}

const std::string& GetTag(const entt::registry& registry, entt::entity entity) {
  static const std::string kEmptyTag;
  if (const auto* tag = registry.try_get<engine::ecs::TagComponent>(entity)) {
    return tag->tag;
  }
  return kEmptyTag;
}

glm::vec2 GetCenter(const entt::registry& registry, entt::entity entity) {
  const auto& transform = registry.get<engine::ecs::TransformComponent>(entity);
  const auto& collider = registry.get<engine::ecs::ColliderComponent>(entity);
  const auto aabb = engine::ecs::GetWorldAabb(transform, collider);
  return aabb.position + aabb.size / 2.0f;
}

}  // namespace

void PlayerSystem::HandleInput(
    entt::registry& registry,
    const engine::input::InputManager& input_manager) const {
  auto view = registry.view<PlayerComponent, PlayerInputComponent,
                            engine::ecs::PhysicsComponent,
                            engine::ecs::SpriteComponent,
                            engine::ecs::AnimationComponent>();
  for (const auto entity : view) {
    auto& player = view.get<PlayerComponent>(entity);
    auto& input = view.get<PlayerInputComponent>(entity);
    auto& physics = view.get<engine::ecs::PhysicsComponent>(entity);
    auto& sprite = view.get<engine::ecs::SpriteComponent>(entity);
    auto& animation = view.get<engine::ecs::AnimationComponent>(entity);
    auto* collider = registry.try_get<engine::ecs::ColliderComponent>(entity);

    input.move_left = input_manager.IsActionDown("move_left");
    input.move_right = input_manager.IsActionDown("move_right");
    input.move_up = input_manager.IsActionDown("move_up");
    input.move_down = input_manager.IsActionDown("move_down");
    input.jump_pressed = input_manager.IsActionPressed("jump");

    if (player.state == PlayerState::kDead || player.state == PlayerState::kHurt) {
      continue;
    }

    if (player.state == PlayerState::kIdle) {
      if (CanClimb(physics, input) && input.move_up) {
        EnterState(registry, entity, player, physics, sprite, animation,
                   collider, PlayerState::kClimb);
        continue;
      }
      if (engine::ecs::HasCollisionFlag(physics,
                                        engine::ecs::PhysicsComponent::kIsOnTopLadder) &&
          input.move_down) {
        if (auto* transform = registry.try_get<engine::ecs::TransformComponent>(
                entity)) {
          transform->position += glm::vec2(0.0f, 2.0f);
        }
        EnterState(registry, entity, player, physics, sprite, animation,
                   collider, PlayerState::kClimb);
        continue;
      }
      if (input.move_left || input.move_right) {
        EnterState(registry, entity, player, physics, sprite, animation,
                   collider, PlayerState::kWalk);
        continue;
      }
      if (input.jump_pressed && IsOnGround(player, physics)) {
        EnterState(registry, entity, player, physics, sprite, animation,
                   collider, PlayerState::kJump);
      }
      continue;
    }

    if (player.state == PlayerState::kWalk || player.state == PlayerState::kJump ||
        player.state == PlayerState::kFall) {
      if (input.move_left) {
        if (physics.velocity.x > 0.0f) {
          physics.velocity.x = 0.0f;
        }
        physics.force += glm::vec2(-player.move_force, 0.0f);
        sprite.sprite.SetFlipped(true);
      } else if (input.move_right) {
        if (physics.velocity.x < 0.0f) {
          physics.velocity.x = 0.0f;
        }
        physics.force += glm::vec2(player.move_force, 0.0f);
        sprite.sprite.SetFlipped(false);
      }
    }

    if (player.state == PlayerState::kWalk) {
      if (input.jump_pressed && IsOnGround(player, physics)) {
        EnterState(registry, entity, player, physics, sprite, animation,
                   collider, PlayerState::kJump);
        continue;
      }
      if (!input.move_left && !input.move_right) {
        EnterState(registry, entity, player, physics, sprite, animation,
                   collider, PlayerState::kIdle);
        continue;
      }
      if (CanClimb(physics, input) && input.move_up) {
        EnterState(registry, entity, player, physics, sprite, animation,
                   collider, PlayerState::kClimb);
      }
      continue;
    }

    if (player.state == PlayerState::kJump || player.state == PlayerState::kFall) {
      if (CanClimb(physics, input) && (input.move_up || input.move_down)) {
        EnterState(registry, entity, player, physics, sprite, animation,
                   collider, PlayerState::kClimb);
      }
      continue;
    }

    if (player.state == PlayerState::kClimb) {
      physics.velocity.y =
          input.move_up ? -player.climb_speed : input.move_down ? player.climb_speed : 0.0f;
      physics.velocity.x = input.move_left
                               ? -player.climb_speed
                               : input.move_right ? player.climb_speed : 0.0f;
      animation.is_playing =
          input.move_up || input.move_down || input.move_left || input.move_right;
      if (input.jump_pressed) {
        EnterState(registry, entity, player, physics, sprite, animation,
                   collider, PlayerState::kJump);
      }
    }
  }
}

void PlayerSystem::Update(entt::registry& registry, float delta_time) const {
  auto view = registry.view<PlayerComponent, PlayerInputComponent,
                            engine::ecs::PhysicsComponent,
                            engine::ecs::SpriteComponent,
                            engine::ecs::AnimationComponent,
                            engine::ecs::HealthComponent>();
  for (const auto entity : view) {
    auto& player = view.get<PlayerComponent>(entity);
    auto& input = view.get<PlayerInputComponent>(entity);
    auto& physics = view.get<engine::ecs::PhysicsComponent>(entity);
    auto& sprite = view.get<engine::ecs::SpriteComponent>(entity);
    auto& animation = view.get<engine::ecs::AnimationComponent>(entity);
    auto& health = view.get<engine::ecs::HealthComponent>(entity);
    auto* collider = registry.try_get<engine::ecs::ColliderComponent>(entity);

    player.state_timer += delta_time;
    if (engine::ecs::HasCollisionFlag(physics,
                                      engine::ecs::PhysicsComponent::kCollidedBelow)) {
      player.coyote_timer = 0.0f;
    } else {
      player.coyote_timer += delta_time;
    }

    if (health.is_invincible) {
      player.flash_timer += delta_time;
      if (player.flash_timer >= 2.0f * player.flash_interval) {
        player.flash_timer -= 2.0f * player.flash_interval;
      }
      sprite.hidden = player.flash_timer < player.flash_interval;
    } else {
      player.flash_timer = 0.0f;
      sprite.hidden = false;
    }

    switch (player.state) {
      case PlayerState::kIdle:
        physics.velocity.x *= player.friction_factor;
        if (!IsOnGround(player, physics)) {
          EnterState(registry, entity, player, physics, sprite, animation,
                     collider, PlayerState::kFall);
        }
        break;
      case PlayerState::kWalk:
        physics.velocity.x =
            glm::clamp(physics.velocity.x, -player.max_speed, player.max_speed);
        if (!IsOnGround(player, physics)) {
          EnterState(registry, entity, player, physics, sprite, animation,
                     collider, PlayerState::kFall);
        }
        break;
      case PlayerState::kJump:
        physics.velocity.x =
            glm::clamp(physics.velocity.x, -player.max_speed, player.max_speed);
        if (physics.velocity.y >= 0.0f) {
          EnterState(registry, entity, player, physics, sprite, animation,
                     collider, PlayerState::kFall);
        }
        break;
      case PlayerState::kFall:
        physics.velocity.x =
            glm::clamp(physics.velocity.x, -player.max_speed, player.max_speed);
        if (engine::ecs::HasCollisionFlag(physics,
                                          engine::ecs::PhysicsComponent::kCollidedBelow)) {
          const auto next_state =
              std::abs(physics.velocity.x) < 1.0f ? PlayerState::kIdle
                                                  : PlayerState::kWalk;
          EnterState(registry, entity, player, physics, sprite, animation,
                     collider, next_state);
        }
        break;
      case PlayerState::kClimb:
        if (engine::ecs::HasCollisionFlag(physics,
                                          engine::ecs::PhysicsComponent::kCollidedBelow)) {
          EnterState(registry, entity, player, physics, sprite, animation,
                     collider, PlayerState::kIdle);
        } else if (!engine::ecs::HasCollisionFlag(
                       physics, engine::ecs::PhysicsComponent::kCollidedLadder)) {
          EnterState(registry, entity, player, physics, sprite, animation,
                     collider, PlayerState::kFall);
        }
        break;
      case PlayerState::kHurt:
        if (engine::ecs::HasCollisionFlag(physics,
                                          engine::ecs::PhysicsComponent::kCollidedBelow)) {
          EnterState(registry, entity, player, physics, sprite, animation,
                     collider, PlayerState::kIdle);
        } else if (player.state_timer > player.stunned_duration) {
          EnterState(registry, entity, player, physics, sprite, animation,
                     collider, PlayerState::kFall);
        }
        break;
      case PlayerState::kDead:
        break;
    }

    input.jump_pressed = false;
  }
}

bool PlayerSystem::ApplyDamage(entt::registry& registry, entt::entity player_entity,
                               int damage) const {
  if (!registry.valid(player_entity) ||
      !registry.all_of<PlayerComponent, engine::ecs::PhysicsComponent,
                       engine::ecs::SpriteComponent,
                       engine::ecs::AnimationComponent,
                       engine::ecs::HealthComponent>(player_entity) ||
      damage <= 0) {
    return false;
  }
  auto& player = registry.get<PlayerComponent>(player_entity);
  auto& physics = registry.get<engine::ecs::PhysicsComponent>(player_entity);
  auto& sprite = registry.get<engine::ecs::SpriteComponent>(player_entity);
  auto& animation = registry.get<engine::ecs::AnimationComponent>(player_entity);
  auto& health = registry.get<engine::ecs::HealthComponent>(player_entity);
  auto* collider = registry.try_get<engine::ecs::ColliderComponent>(player_entity);
  if (player.is_dead || health.is_invincible) {
    return false;
  }

  health.current_health = std::max(0, health.current_health - damage);
  health.is_invincible = true;
  health.invincibility_timer = health.invincibility_duration;
  if (health.current_health > 0) {
    EnterState(registry, player_entity, player, physics, sprite, animation,
               collider, PlayerState::kHurt);
  } else {
    player.is_dead = true;
    EnterState(registry, player_entity, player, physics, sprite, animation,
               collider, PlayerState::kDead);
  }
  return true;
}

void PlayerSystem::EnterState(entt::registry&, entt::entity,
                              PlayerComponent& player,
                              engine::ecs::PhysicsComponent& physics,
                              engine::ecs::SpriteComponent& sprite,
                              engine::ecs::AnimationComponent& animation,
                              engine::ecs::ColliderComponent* collider,
                              PlayerState new_state) const {
  if (player.state == PlayerState::kClimb && new_state != PlayerState::kClimb) {
    physics.use_gravity = true;
  }

  player.state = new_state;
  player.state_timer = 0.0f;
  switch (new_state) {
    case PlayerState::kIdle:
      PlayAnimation(animation, "idle");
      break;
    case PlayerState::kWalk:
      PlayAnimation(animation, "walk");
      break;
    case PlayerState::kJump:
      physics.use_gravity = true;
      physics.velocity.y = -player.jump_velocity;
      PlayAnimation(animation, "jump");
      break;
    case PlayerState::kFall:
      physics.use_gravity = true;
      PlayAnimation(animation, "fall");
      break;
    case PlayerState::kClimb:
      physics.use_gravity = false;
      PlayAnimation(animation, "climb");
      break;
    case PlayerState::kHurt: {
      PlayAnimation(animation, "hurt");
      glm::vec2 knockback_velocity = {-100.0f, -150.0f};
      if (sprite.sprite.IsFlipped()) {
        knockback_velocity.x = -knockback_velocity.x;
      }
      physics.velocity = knockback_velocity;
      break;
    }
    case PlayerState::kDead:
      PlayAnimation(animation, "hurt");
      physics.velocity = {0.0f, -200.0f};
      if (collider != nullptr) {
        collider->is_active = false;
      }
      break;
  }
}

bool PlayerSystem::IsOnGround(const PlayerComponent& player,
                              const engine::ecs::PhysicsComponent& physics) const {
  return player.coyote_timer <= player.coyote_time ||
         engine::ecs::HasCollisionFlag(
             physics, engine::ecs::PhysicsComponent::kCollidedBelow);
}

bool PlayerSystem::CanClimb(const engine::ecs::PhysicsComponent& physics,
                            const PlayerInputComponent& input) const {
  return engine::ecs::HasCollisionFlag(
             physics, engine::ecs::PhysicsComponent::kCollidedLadder) &&
         (input.move_up || input.move_down);
}

void PlayerSystem::PlayAnimation(engine::ecs::AnimationComponent& animation,
                                 const std::string& animation_name) const {
  if (animation.current_animation == animation_name && animation.is_playing) {
    return;
  }
  animation.current_animation = animation_name;
  animation.animation_timer = 0.0f;
  animation.is_playing = true;
}

void AiSystem::Update(entt::registry& registry, float delta_time) const {
  auto view = registry.view<AiComponent, engine::ecs::TransformComponent,
                            engine::ecs::PhysicsComponent,
                            engine::ecs::SpriteComponent,
                            engine::ecs::AnimationComponent>();
  for (const auto entity : view) {
    auto& ai = view.get<AiComponent>(entity);
    auto& transform = view.get<engine::ecs::TransformComponent>(entity);
    auto& physics = view.get<engine::ecs::PhysicsComponent>(entity);
    auto& sprite = view.get<engine::ecs::SpriteComponent>(entity);
    auto& animation = view.get<engine::ecs::AnimationComponent>(entity);

    switch (ai.behavior) {
      case AiBehaviorType::kUpDown:
        physics.use_gravity = false;
        PlayAnimation(animation, "fly");
        if (engine::ecs::HasCollisionFlag(
                physics, engine::ecs::PhysicsComponent::kCollidedAbove) ||
            transform.position.y <= ai.min_bound) {
          physics.velocity.y = ai.speed;
        } else if (engine::ecs::HasCollisionFlag(
                       physics, engine::ecs::PhysicsComponent::kCollidedBelow) ||
                   transform.position.y >= ai.max_bound) {
          physics.velocity.y = -ai.speed;
        }
        break;
      case AiBehaviorType::kPatrol:
        PlayAnimation(animation, "walk");
        if (engine::ecs::HasCollisionFlag(
                physics, engine::ecs::PhysicsComponent::kCollidedRight) ||
            transform.position.x >= ai.max_bound) {
          physics.velocity.x = -ai.speed;
        } else if (engine::ecs::HasCollisionFlag(
                       physics, engine::ecs::PhysicsComponent::kCollidedLeft) ||
                   transform.position.x <= ai.min_bound) {
          physics.velocity.x = ai.speed;
        }
        sprite.sprite.SetFlipped(physics.velocity.x < 0.0f);
        break;
      case AiBehaviorType::kJump:
        ai.jump_timer += delta_time;
        if (engine::ecs::HasCollisionFlag(
                physics, engine::ecs::PhysicsComponent::kCollidedBelow)) {
          PlayAnimation(animation, "idle");
          if (ai.jump_timer >= ai.jump_interval) {
            ai.jump_timer = 0.0f;
            float jump_velocity_x = physics.velocity.x >= 0.0f ? 100.0f : -100.0f;
            if (transform.position.x >= ai.max_bound ||
                engine::ecs::HasCollisionFlag(
                    physics, engine::ecs::PhysicsComponent::kCollidedRight)) {
              jump_velocity_x = -100.0f;
            } else if (transform.position.x <= ai.min_bound ||
                       engine::ecs::HasCollisionFlag(
                           physics, engine::ecs::PhysicsComponent::kCollidedLeft)) {
              jump_velocity_x = 100.0f;
            }
            physics.velocity = {jump_velocity_x, -300.0f};
            sprite.sprite.SetFlipped(jump_velocity_x < 0.0f);
          }
        } else if (physics.velocity.y < 0.0f) {
          PlayAnimation(animation, "jump");
        } else {
          PlayAnimation(animation, "fall");
        }
        break;
      case AiBehaviorType::kNone:
        break;
    }
  }
}

void AiSystem::PlayAnimation(engine::ecs::AnimationComponent& animation,
                             const std::string& animation_name) const {
  if (animation.current_animation == animation_name && animation.is_playing) {
    return;
  }
  animation.current_animation = animation_name;
  animation.animation_timer = 0.0f;
  animation.is_playing = true;
}

RuleEvents GameRuleSystem::Update(
    entt::registry& registry, entt::entity player_entity,
    const std::vector<engine::ecs::CollisionPair>& collisions,
    const std::vector<engine::ecs::TileTriggerEvent>& tile_triggers) const {
  RuleEvents events;
  if (!registry.valid(player_entity)) {
    return events;
  }

  std::unordered_set<entt::entity> marked_destroy;
  for (const auto& collision : collisions) {
    entt::entity first = collision.first;
    entt::entity second = collision.second;
    if (first != player_entity && second != player_entity) {
      continue;
    }

    const entt::entity other = first == player_entity ? second : first;
    const auto& other_tag = GetTag(registry, other);
    const auto& other_name = GetName(registry, other);
    if (other_tag == "enemy") {
      if (!registry.all_of<engine::ecs::ColliderComponent>(player_entity) ||
          !registry.all_of<engine::ecs::ColliderComponent>(other)) {
        continue;
      }
      const auto player_aabb = engine::ecs::GetWorldAabb(
          registry.get<engine::ecs::TransformComponent>(player_entity),
          registry.get<engine::ecs::ColliderComponent>(player_entity));
      const auto enemy_aabb = engine::ecs::GetWorldAabb(
          registry.get<engine::ecs::TransformComponent>(other),
          registry.get<engine::ecs::ColliderComponent>(other));
      const glm::vec2 player_center = player_aabb.position + player_aabb.size / 2.0f;
      const glm::vec2 enemy_center = enemy_aabb.position + enemy_aabb.size / 2.0f;
      const glm::vec2 overlap =
          glm::vec2(player_aabb.size / 2.0f + enemy_aabb.size / 2.0f) -
          glm::abs(player_center - enemy_center);
      if (overlap.x > overlap.y && player_center.y < enemy_center.y) {
        if (auto* health = registry.try_get<engine::ecs::HealthComponent>(other)) {
          health->current_health = std::max(0, health->current_health - 1);
          if (health->current_health == 0 && !marked_destroy.contains(other)) {
            marked_destroy.insert(other);
            events.destroy_entities.push_back(other);
            events.score_delta += 10;
            events.effects.push_back({GetCenter(registry, other), "enemy"});
          }
        }
        registry.get<engine::ecs::PhysicsComponent>(player_entity).velocity.y =
            -300.0f;
      } else {
        ++events.player_damage;
      }
      continue;
    }

    if (other_tag == "item") {
      if (other_name == "fruit") {
        ++events.heal_amount;
      } else if (other_name == "gem") {
        events.score_delta += 5;
      }
      if (!marked_destroy.contains(other)) {
        marked_destroy.insert(other);
        events.destroy_entities.push_back(other);
        if (registry.all_of<engine::ecs::ColliderComponent>(other)) {
          events.effects.push_back({GetCenter(registry, other), "item"});
        }
      }
      continue;
    }

    if (other_tag == "hazard") {
      ++events.player_damage;
      continue;
    }

    if (other_tag == "next_level") {
      events.next_level_name = other_name;
      continue;
    }

    if (other_name == "win") {
      events.request_win = true;
    }
  }

  for (const auto& tile_trigger : tile_triggers) {
    if (tile_trigger.entity == player_entity &&
        tile_trigger.tile_type == engine::ecs::TileType::kHazard) {
      ++events.player_damage;
    }
  }

  return events;
}

}  // namespace game::ecs