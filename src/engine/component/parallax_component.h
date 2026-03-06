// Copyright Sylar129

#pragma once

#include <string>

#include "engine/component/component.h"
#include "engine/render/sprite.h"
#include "glm/vec2.hpp"

namespace engine::component {
class TransformComponent;

class ParallaxComponent final : public Component {
  friend class engine::object::GameObject;

 public:
  ParallaxComponent(const std::string& texture_id,
                    const glm::vec2& scroll_factor, const glm::bvec2& repeat);

  void SetSprite(const engine::render::Sprite& sprite) { sprite_ = sprite; }
  void SetScrollFactor(const glm::vec2& factor) { scroll_factor_ = factor; }
  void SetRepeat(const glm::bvec2& repeat) { repeat_ = repeat; }
  void SetHidden(bool hidden) { is_hidden_ = hidden; }

  const engine::render::Sprite& GetSprite() const { return sprite_; }
  const glm::vec2& GetScrollFactor() const { return scroll_factor_; }
  const glm::bvec2& GetRepeat() const { return repeat_; }
  bool IsHidden() const { return is_hidden_; }

 protected:
  void Update(float, engine::core::Context&) override {}
  void Init() override;
  void Render(engine::core::Context& context) override;

 private:
  TransformComponent* transform_ = nullptr;

  engine::render::Sprite sprite_;
  glm::vec2 scroll_factor_;  ///< @brief 滚动速度因子 (0=静止, 1=随相机移动,
                             ///< <1=比相机慢)
  glm::bvec2 repeat_;
  bool is_hidden_ = false;
};

}  // namespace engine::component
