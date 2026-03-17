// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "engine/render/sprite.h"
#include "engine/ui/ui_element.h"
#include "state/ui_state.h"

namespace engine::core {
class Context;
}

namespace engine::ui {

class UIInteractive : public UIElement {
 public:
  UIInteractive(engine::core::Context& context,
                const glm::vec2& position = {0.0f, 0.0f},
                const glm::vec2& size = {0.0f, 0.0f});
  ~UIInteractive() override;

  virtual void Clicked() {}

  void AddSprite(const std::string& name,
                 std::unique_ptr<engine::render::Sprite> sprite);
  void SetSprite(const std::string& name);
  void AddSound(const std::string& name, const std::string& path);
  void PlaySound(const std::string& name);

  void SetState(std::unique_ptr<engine::ui::state::UIState> state);
  engine::ui::state::UIState* GetState() const { return state_.get(); }

  void SetInteractive(bool interactive) { interactive_ = interactive; }
  bool IsInteractive() const { return interactive_; }

  bool HandleInput(engine::core::Context& context) override;
  void Render(engine::core::Context& context) override;

 protected:
  engine::core::Context& context_;
  std::unique_ptr<engine::ui::state::UIState> state_;
  std::unordered_map<std::string, std::unique_ptr<engine::render::Sprite>>
      sprites_;
  std::unordered_map<std::string, std::string> sounds_;
  engine::render::Sprite* current_sprite_ = nullptr;
  bool interactive_ = true;
};

}  // namespace engine::ui
