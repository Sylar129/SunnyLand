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
  ENGINE_TRACE("UIManager created with root UIPanel.");
}

bool UIManager::init(const glm::vec2& window_size) {
  root_element_->setSize(window_size);
  ENGINE_TRACE("UIManager initialized with window size ({}, {})", window_size.x,
               window_size.y);
  return true;
}

void UIManager::addElement(std::unique_ptr<UIElement> element) {
  if (!root_element_) {
    ENGINE_TRACE("Cannot add UIElement because root_element_ is null.");
    return;
  }
  root_element_->addChild(std::move(element));
}

void UIManager::clearElements() {
  if (root_element_) {
    root_element_->removeAllChildren();
    ENGINE_TRACE("All UIElements removed from root UIPanel.");
  }
}

bool UIManager::handleInput(engine::core::Context& context) {
  if (root_element_ && root_element_->isVisible()) {
    if (root_element_->handleInput(context)) return true;
  }
  return false;
}

void UIManager::update(float delta_time, engine::core::Context& context) {
  if (root_element_ && root_element_->isVisible()) {
    root_element_->update(delta_time, context);
  }
}

void UIManager::render(engine::core::Context& context) {
  if (root_element_ && root_element_->isVisible()) {
    root_element_->render(context);
  }
}

UIPanel* UIManager::getRootElement() const { return root_element_.get(); }

}  // namespace engine::ui
