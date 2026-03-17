// Copyright Sylar129

#include "engine/ui/ui_interactive.h"

#include "engine/core/context.h"
#include "engine/render/renderer.h"
#include "engine/resource/resource_manager.h"
#include "engine/ui/state/ui_state.h"
#include "log.h"

namespace engine::ui {

UIInteractive::~UIInteractive() = default;

UIInteractive::UIInteractive(engine::core::Context& context,
                             const glm::vec2& position, const glm::vec2& size)
    : UIElement(position, size), context_(context) {}

void UIInteractive::SetState(
    std::unique_ptr<engine::ui::state::UIState> state) {
  if (!state) {
    ENGINE_LOG_WARN(
        "Trying to set null state for UIInteractive. Operation ignored.");
    return;
  }

  state_ = std::move(state);
  state_->Enter();
}

void UIInteractive::AddSprite(const std::string& name,
                              std::unique_ptr<engine::render::Sprite> sprite) {
  if (size_.x == 0.0f && size_.y == 0.0f) {
    size_ =
        context_.GetResourceManager().GetTextureSize(sprite->GetTextureId());
  }
  sprites_[name] = std::move(sprite);
}

void UIInteractive::SetSprite(const std::string& name) {
  if (sprites_.find(name) != sprites_.end()) {
    current_sprite_ = sprites_[name].get();
  } else {
    ENGINE_LOG_WARN("Sprite '{}' not found", name);
  }
}

void UIInteractive::AddSound(const std::string& name, const std::string& path) {
  sounds_[name] = path;
}

void UIInteractive::PlaySound(const std::string& name) {
  if (sounds_.find(name) != sounds_.end()) {
    // context_.getAudioPlayer().playSound(sounds_[name]);
  } else {
    ENGINE_LOG_WARN("Sound '{}' not found", name);
  }
}

bool UIInteractive::HandleInput(engine::core::Context& context) {
  if (UIElement::HandleInput(context)) {
    return true;
  }

  if (state_ && interactive_) {
    if (auto next_state = state_->HandleInput(context); next_state) {
      SetState(std::move(next_state));
      return true;
    }
  }
  return false;
}

void UIInteractive::Render(engine::core::Context& context) {
  if (!visible_) return;

  context.GetRenderer().DrawUISprite(*current_sprite_, GetScreenPosition(),
                                     size_);

  UIElement::Render(context);
}

}  // namespace engine::ui
