// Copyright Sylar129

#include "engine/ui/ui_button.h"

#include "engine/ui/state/ui_normal_state.h"
#include "log.h"

namespace engine::ui {
UIButton::UIButton(engine::core::Context& context,
                   const std::string& normal_sprite_id,
                   const std::string& hover_sprite_id,
                   const std::string& pressed_sprite_id,
                   const glm::vec2& position, const glm::vec2& size,
                   std::function<void()> callback)
    : UIInteractive(context, position, size), callback_(std::move(callback)) {
  AddSprite("normal",
            std::make_unique<engine::render::Sprite>(normal_sprite_id));
  AddSprite("hover", std::make_unique<engine::render::Sprite>(hover_sprite_id));
  AddSprite("pressed",
            std::make_unique<engine::render::Sprite>(pressed_sprite_id));

  SetState(std::make_unique<engine::ui::state::UINormalState>(this));

  AddSound("hover", "assets/audio/button_hover.wav");
  AddSound("pressed", "assets/audio/button_click.wav");
  ENGINE_TRACE(
      "UIButton constructed with normal '{}', hover '{}', pressed '{}' sprites",
      normal_sprite_id, hover_sprite_id, pressed_sprite_id);
}

void UIButton::Clicked() {
  if (callback_) callback_();
}

}  // namespace engine::ui
