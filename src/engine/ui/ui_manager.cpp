// Copyright Sylar129

#include "engine/ui/ui_manager.h"

#include "engine/ui/ui_element.h"
#include "engine/ui/ui_panel.h"
#include "log.h"

namespace engine::ui {

UIManager::~UIManager() = default;

UIManager::UIManager() {
  root_element_ =
      std::make_unique<UIPanel>(glm::vec2{0.0f, 0.0f}, glm::vec2{0.0f, 0.0f});
  ENGINE_LOG_TRACE("UIManager created with root UIPanel.");
}

bool UIManager::Init(const glm::vec2& window_size) {
  root_element_->SetSize(window_size);
  ENGINE_LOG_TRACE("UIManager initialized with window size ({}, {})",
                   window_size.x, window_size.y);
  return true;
}

void UIManager::AddElement(std::unique_ptr<UIElement> element) {
  if (!root_element_) {
    ENGINE_LOG_TRACE("Cannot add UIElement because root_element_ is null.");
    return;
  }
  root_element_->AddChild(std::move(element));
}

void UIManager::ClearElements() {
  if (root_element_) {
    root_element_->RemoveAllChildren();
    ENGINE_LOG_TRACE("All UIElements removed from root UIPanel.");
  }
}

bool UIManager::HandleInput(engine::core::Context& context) {
  if (root_element_ && root_element_->IsVisible()) {
    if (root_element_->HandleInput(context)) return true;
  }
  return false;
}

void UIManager::Update(float delta_time, engine::core::Context& context) {
  if (root_element_ && root_element_->IsVisible()) {
    root_element_->Update(delta_time, context);
  }
}

void UIManager::Render(engine::core::Context& context) {
  if (root_element_ && root_element_->IsVisible()) {
    root_element_->Render(context);
  }
}

UIPanel* UIManager::GetRootElement() const { return root_element_.get(); }

}  // namespace engine::ui
