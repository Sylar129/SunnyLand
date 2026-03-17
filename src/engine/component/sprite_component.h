// Copyright Sylar129

#pragma once

#include <optional>
#include <string>

#include "SDL3/SDL_rect.h"
#include "engine/component/component.h"
#include "engine/render/sprite.h"
#include "glm/vec2.hpp"
#include "utils/alignment.h"

namespace engine::resource {
class ResourceManager;
}

namespace engine::component {
class TransformComponent;

class SpriteComponent final : public engine::component::Component {
  friend class engine::object::GameObject;

 public:
  SpriteComponent(
      const std::string& texture_id,
      engine::resource::ResourceManager& resource_manager,
      engine::utils::Alignment alignment = engine::utils::Alignment::kNone,
      const std::optional<SDL_FRect>& source_rect_opt = std::nullopt,
      bool is_flipped = false);
  SpriteComponent(
      engine::render::Sprite&& sprite,
      engine::resource::ResourceManager& resource_manager,
      engine::utils::Alignment alignment = engine::utils::Alignment::kNone);
  ~SpriteComponent() override = default;

  void UpdateOffset();

  const engine::render::Sprite& GetSprite() const { return sprite_; }
  const std::string& GetTextureId() const { return sprite_.GetTextureId(); }
  bool IsFlipped() const { return sprite_.IsFlipped(); }
  bool IsHidden() const { return is_hidden_; }
  const glm::vec2& GetSpriteSize() const { return sprite_size_; }
  const glm::vec2& GetOffset() const { return offset_; }
  engine::utils::Alignment GetAlignment() const { return alignment_; }

  void SetSpriteById(
      const std::string& texture_id,
      const std::optional<SDL_FRect>& source_rect_opt = std::nullopt);
  void SetFlipped(bool flipped) { sprite_.SetFlipped(flipped); }
  void SetHidden(bool hidden) { is_hidden_ = hidden; }
  void SetSourceRect(const std::optional<SDL_FRect>& source_rect_opt);
  void SetAlignment(engine::utils::Alignment anchor);

 private:
  void UpdateSpriteSize();

  void Init() override;
  void Update(float, engine::core::Context&) override {}
  void Render(engine::core::Context& context) override;

  engine::resource::ResourceManager& resource_manager_;
  TransformComponent* transform_ = nullptr;

  engine::render::Sprite sprite_;
  engine::utils::Alignment alignment_ = engine::utils::Alignment::kNone;
  glm::vec2 sprite_size_ = {0.0f, 0.0f};
  glm::vec2 offset_ = {0.0f, 0.0f};
  bool is_hidden_ = false;
};

}  // namespace engine::component
