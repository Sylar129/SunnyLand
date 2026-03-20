// Copyright Sylar129

#include "engine/ui/ui_button.h"

#include "engine/ui/state/ui_normal_state.h"
#include "utils/log.h"

namespace engine::ui {

UIButton::UIButton(core::Context& context, const std::string& normal_texture_id,
                   const std::string& hover_texture_id,
                   const std::string& pressed_texture_id,
                   const glm::vec2& position, const glm::vec2& size,
                   std::function<void()> callback)
    : UIInteractive(context, position, size), callback_(std::move(callback)) {
  AddTexture("normal", std::make_unique<render::Texture>(normal_texture_id));
  AddTexture("hover", std::make_unique<render::Texture>(hover_texture_id));
  AddTexture("pressed", std::make_unique<render::Texture>(pressed_texture_id));

  SetState(std::make_unique<ui::state::UINormalState>(this));

  AddSound("hover", "assets/audio/button_hover.wav");
  AddSound("pressed", "assets/audio/button_click.wav");
  ENGINE_LOG_TRACE(
      "UIButton constructed with normal '{}', hover '{}', pressed '{}' textures",
      normal_texture_id, hover_texture_id, pressed_texture_id);
}

void UIButton::Clicked() {
  if (callback_) callback_();
}

}  // namespace engine::ui
