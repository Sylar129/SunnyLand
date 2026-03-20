// Copyright Sylar129

#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "engine/render/texture.h"
#include "engine/ui/ui_element.h"
#include "state/ui_state.h"

namespace engine::core {
class Context;
}

namespace engine::ui {

class UIInteractive : public UIElement {
 public:
  UIInteractive(core::Context& context,
                const glm::vec2& position = {0.0f, 0.0f},
                const glm::vec2& size = {0.0f, 0.0f});
  ~UIInteractive() override;

  virtual void Clicked() {}

  void AddSprite(const std::string& name,
                 std::unique_ptr<render::Texture> sprite);
  void SetSprite(const std::string& name);
  void AddSound(const std::string& id, const std::string& path);
  void PlaySound(const std::string& id);

  void SetState(std::unique_ptr<state::UIState> state);
  state::UIState* GetState() const { return state_.get(); }

  void SetInteractive(bool interactive) { interactive_ = interactive; }
  bool IsInteractive() const { return interactive_; }

  bool HandleInput(core::Context& context) override;
  void Render(core::Context& context) override;

 protected:
  core::Context& context_;
  std::unique_ptr<state::UIState> state_;
  std::unordered_map<std::string, std::unique_ptr<render::Texture>> sprites_;
  render::Texture* current_sprite_ = nullptr;
  bool interactive_ = true;
};

}  // namespace engine::ui
