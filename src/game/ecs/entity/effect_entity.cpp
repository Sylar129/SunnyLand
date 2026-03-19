// Copyright Sylar129

#include "game/ecs/entity/effect_entity.h"

#include <memory>

#include "engine/ecs/components.h"
#include "engine/render/animation.h"
#include "engine/resource/resource_manager.h"
#include "utils/alignment.h"

namespace game::ecs::entity {

entt::entity CreateEffectEntity(entt::registry& registry,
                                engine::resource::ResourceManager& resource_manager,
                                const glm::vec2& center_pos,
                                const std::string& tag) {
  const auto effect_entity = registry.create();
  registry.emplace<engine::ecs::NameComponent>(effect_entity, "effect_" + tag);
  registry.emplace<engine::ecs::RenderOrderComponent>(effect_entity, 10);
  registry.emplace<engine::ecs::TransformComponent>(
      effect_entity, center_pos, glm::vec2(1.0f, 1.0f), 0.0f);

  auto animation = std::make_shared<engine::render::Animation>("effect", false);
  engine::ecs::SpriteComponent sprite;
  sprite.alignment = utils::Alignment::kCenter;
  if (tag == "enemy") {
    sprite.sprite.SetTextureId("assets/textures/FX/enemy-deadth.png");
    for (int i = 0; i < 5; ++i) {
      animation->AddFrame({static_cast<float>(i * 40), 0.0f, 40.0f, 41.0f},
                          0.1f);
    }
  } else if (tag == "item") {
    sprite.sprite.SetTextureId("assets/textures/FX/item-feedback.png");
    for (int i = 0; i < 4; ++i) {
      animation->AddFrame({static_cast<float>(i * 32), 0.0f, 32.0f, 32.0f},
                          0.1f);
    }
  } else {
    registry.destroy(effect_entity);
    return entt::null;
  }

  sprite.sprite_size =
      resource_manager.GetTextureSize(sprite.sprite.GetTextureId());
  registry.emplace<engine::ecs::SpriteComponent>(effect_entity, std::move(sprite));
  auto& animation_component =
      registry.emplace<engine::ecs::AnimationComponent>(effect_entity);
  animation_component.animations.emplace("effect", animation);
  animation_component.current_animation = "effect";
  animation_component.animation_timer = 0.0f;
  animation_component.is_playing = true;
  animation_component.is_one_shot_removal = true;
  return effect_entity;
}

}  // namespace game::ecs::entity