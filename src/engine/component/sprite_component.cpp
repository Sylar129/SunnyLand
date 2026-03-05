// Copyright Sylar129

#include "engine/component/sprite_component.h"

#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/object/game_object.h"
#include "engine/render/renderer.h"
#include "engine/resource/resource_manager.h"
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
  ENGINE_TRACE("Creating SpriteComponent, texture id: {}", texture_id);
}

void SpriteComponent::Init() {
  if (!owner_) {
    ENGINE_ERROR("SpriteComponent 在初始化前未设置所有者。");
    return;
  }
  transform_ = owner_->GetComponent<TransformComponent>();
  if (!transform_) {
    ENGINE_WARN(
        "GameObject '{}' 上的 SpriteComponent 需要一个 "
        "TransformComponent，但未找到。",
        owner_->GetName());
    // Sprite没有Transform无法计算偏移和渲染，直接返回
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
  // 计算精灵左上角相对于 TransformComponent::position_ 的偏移
  switch (alignment_) {
    case engine::utils::Alignment::TOP_LEFT:
      offset_ = glm::vec2{0.0f, 0.0f} * scale;
      break;
    case engine::utils::Alignment::TOP_CENTER:
      offset_ = glm::vec2{-sprite_size_.x / 2.0f, 0.0f} * scale;
      break;
    case engine::utils::Alignment::TOP_RIGHT:
      offset_ = glm::vec2{-sprite_size_.x, 0.0f} * scale;
      break;
    case engine::utils::Alignment::CENTER_LEFT:
      offset_ = glm::vec2{0.0f, -sprite_size_.y / 2.0f} * scale;
      break;
    case engine::utils::Alignment::CENTER:
      offset_ =
          glm::vec2{-sprite_size_.x / 2.0f, -sprite_size_.y / 2.0f} * scale;
      break;
    case engine::utils::Alignment::CENTER_RIGHT:
      offset_ = glm::vec2{-sprite_size_.x, -sprite_size_.y / 2.0f} * scale;
      break;
    case engine::utils::Alignment::BOTTOM_LEFT:
      offset_ = glm::vec2{0.0f, -sprite_size_.y} * scale;
      break;
    case engine::utils::Alignment::BOTTOM_CENTER:
      offset_ = glm::vec2{-sprite_size_.x / 2.0f, -sprite_size_.y} * scale;
      break;
    case engine::utils::Alignment::BOTTOM_RIGHT:
      offset_ = glm::vec2{-sprite_size_.x, -sprite_size_.y} * scale;
      break;
    case engine::utils::Alignment::NONE:
    default:
      break;
  }
}

void SpriteComponent::Render(engine::core::Context& context) {
  if (is_hidden_ || !transform_ || !resource_manager_) {
    return;
  }

  // 获取变换信息（考虑偏移量）
  const glm::vec2& pos = transform_->GetPosition() + offset_;
  const glm::vec2& scale = transform_->GetScale();
  float rotation_degrees = transform_->GetRotation();

  // 执行绘制
  context.getRenderer().DrawSprite(context.getCamera(), sprite_, pos, scale,
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
  if (!resource_manager_) {
    ENGINE_ERROR("ResourceManager 为空！无法获取纹理尺寸。");
    return;
  }
  if (sprite_.GetSourceRect().has_value()) {
    const auto& src_rect = sprite_.GetSourceRect().value();
    sprite_size_ = {src_rect.w, src_rect.h};
  } else {
    sprite_size_ = resource_manager_->GetTextureSize(sprite_.GetTextureId());
  }
}

}  // namespace engine::component