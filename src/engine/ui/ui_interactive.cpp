// Copyright Sylar129

#include "engine/ui/ui_interactive.h"

#include "engine/core/context.h"
#include "engine/render/renderer.h"
#include "engine/resource/resource_manager.h"
#include "engine/ui/state/ui_state.h"
#include "utils/log.h"

namespace engine::ui {

UIInteractive::~UIInteractive() = default;

UIInteractive::UIInteractive(core::Context& context, const glm::vec2& position,
                             const glm::vec2& size)
    : UIElement(position, size), context_(context) {}

void UIInteractive::SetState(std::unique_ptr<state::UIState> state) {
  if (!state) {
    ENGINE_LOG_WARN(
        "Trying to set null state for UIInteractive. Operation ignored.");
    return;
  }

  state_ = std::move(state);
  state_->Enter();
}

void UIInteractive::AddTexture(const std::string& name,
                               std::unique_ptr<render::Texture> texture) {
  if (size_.x == 0.0f && size_.y == 0.0f) {
    size_ =
        context_.GetResourceManager().GetTextureSize(texture->GetTextureId());
  }
  textures_[name] = std::move(texture);
}

void UIInteractive::SetTexture(const std::string& name) {
  if (textures_.find(name) != textures_.end()) {
    current_texture_ = textures_[name].get();
  } else {
    ENGINE_LOG_WARN("Texture '{}' not found", name);
  }
}

void UIInteractive::AddSound(const std::string& id, const std::string& path) {
  context_.GetAudioManager().LoadAudio(path, id);
}

void UIInteractive::PlaySound(const std::string& id) {
  context_.GetAudioManager().PlayAudio(id);
}

bool UIInteractive::HandleInput(core::Context& context) {
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

void UIInteractive::Render(core::Context& context) {
  if (!visible_) return;

  context.GetRenderer().DrawUITexture(*current_texture_, GetScreenPosition(),
                                      size_);

  UIElement::Render(context);
}

}  // namespace engine::ui
