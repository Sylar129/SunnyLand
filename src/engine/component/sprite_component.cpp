// Copyright Sylar129

#include "engine/component/sprite_component.h"

#include "assert.h"
#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/object/game_object.h"
#include "engine/render/renderer.h"
#include "engine/resource/resource_manager.h"
#include "engine/utils/assert.h"
#include "log.h"

namespace engine::component {

SpriteComponent::SpriteComponent(
    const std::string& texture_id,
    engine::resource::ResourceManager& resource_manager,
    engine::utils::Alignment alignment,
    const std::optional<SDL_FRect>& source_rect_opt, bool is_flipped)
    : resource_manager_(resource_manager),
      sprite_(texture_id, source_rect_opt, is_flipped),
      alignment_(alignment) {
  ENGINE_LOG_TRACE("Creating SpriteComponent, texture id: {}", texture_id);
}

SpriteComponent::SpriteComponent(
    engine::render::Sprite&& sprite,
    engine::resource::ResourceManager& resource_manager,
    engine::utils::Alignment alignment)
    : resource_manager_(resource_manager),
      sprite_(std::move(sprite)),
      alignment_(alignment) {
  ENGINE_LOG_TRACE("Creating SpriteComponent, texture id: {}",
                   sprite_.GetTextureId());
}

void SpriteComponent::Init() {
  ENGINE_LOG_ASSERT(owner_,
                    "SpriteComponent does not have an owner GameObject!");
  transform_ = owner_->GetComponent<TransformComponent>();
  if (!transform_) {
    ENGINE_LOG_WARN(
        "The SpriteComponent in GameObject '{}' does not have a "
        "TransformComponent ",
        owner_->GetName());
    return;
  }

  UpdateSpriteSize();
  UpdateOffset();
}

void SpriteComponent::SetAlignment(engine::utils::Alignment anchor) {
  alignment_ = anchor;
  UpdateOffset();
}

void SpriteComponent::UpdateOffset() {
  if (sprite_size_.x <= 0 || sprite_size_.y <= 0) {
    offset_ = {0.0f, 0.0f};
    return;
  }
  auto scale = transform_->GetScale();
  switch (alignment_) {
    case engine::utils::Alignment::kTopLeft:
      offset_ = glm::vec2{0.0f, 0.0f} * scale;
      break;
    case engine::utils::Alignment::kTopCenter:
      offset_ = glm::vec2{-sprite_size_.x / 2.0f, 0.0f} * scale;
      break;
    case engine::utils::Alignment::kTopRight:
      offset_ = glm::vec2{-sprite_size_.x, 0.0f} * scale;
      break;
    case engine::utils::Alignment::kCenterLeft:
      offset_ = glm::vec2{0.0f, -sprite_size_.y / 2.0f} * scale;
      break;
    case engine::utils::Alignment::kCenter:
      offset_ =
          glm::vec2{-sprite_size_.x / 2.0f, -sprite_size_.y / 2.0f} * scale;
      break;
    case engine::utils::Alignment::kCenterRight:
      offset_ = glm::vec2{-sprite_size_.x, -sprite_size_.y / 2.0f} * scale;
      break;
    case engine::utils::Alignment::kBottomLeft:
      offset_ = glm::vec2{0.0f, -sprite_size_.y} * scale;
      break;
    case engine::utils::Alignment::kBottomCenter:
      offset_ = glm::vec2{-sprite_size_.x / 2.0f, -sprite_size_.y} * scale;
      break;
    case engine::utils::Alignment::kBottomRight:
      offset_ = glm::vec2{-sprite_size_.x, -sprite_size_.y} * scale;
      break;
    case engine::utils::Alignment::kNone:
    default:
      break;
  }
}

void SpriteComponent::Render(engine::core::Context& context) {
  if (is_hidden_ || !transform_) {
    return;
  }

  const glm::vec2& pos = transform_->GetPosition() + offset_;
  const glm::vec2& scale = transform_->GetScale();
  float rotation_degrees = transform_->GetRotation();

  context.GetRenderer().DrawSprite(context.GetCamera(), sprite_, pos, scale,
                                   rotation_degrees);
}

void SpriteComponent::SetSpriteById(
    const std::string& texture_id,
    const std::optional<SDL_FRect>& source_rect_opt) {
  sprite_.SetTextureId(texture_id);
  sprite_.SetSourceRect(source_rect_opt);

  UpdateSpriteSize();
  UpdateOffset();
}

void SpriteComponent::SetSourceRect(
    const std::optional<SDL_FRect>& source_rect_opt) {
  sprite_.SetSourceRect(source_rect_opt);
  UpdateSpriteSize();
  UpdateOffset();
}

void SpriteComponent::UpdateSpriteSize() {
  if (sprite_.GetSourceRect().has_value()) {
    const auto& src_rect = sprite_.GetSourceRect().value();
    sprite_size_ = {src_rect.w, src_rect.h};
  } else {
    sprite_size_ = resource_manager_.GetTextureSize(sprite_.GetTextureId());
  }
}

}  // namespace engine::component
