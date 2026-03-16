// Copyright Sylar129

#include "engine/ui/ui_element.h"

#include <algorithm>
#include <utility>

#include "engine/core/context.h"

namespace engine::ui {

UIElement::UIElement(const glm::vec2& position, const glm::vec2& size)
    : position_(position), size_(size) {}

bool UIElement::HandleInput(engine::core::Context& context) {
  if (!visible_) return false;

  for (auto it = children_.begin(); it != children_.end();) {
    if (*it && !(*it)->IsNeedRemove()) {
      if ((*it)->HandleInput(context)) return true;
      ++it;
    } else {
      it = children_.erase(it);
    }
  }
  return false;
}

void UIElement::Update(float delta_time, engine::core::Context& context) {
  if (!visible_) return;

  for (auto it = children_.begin(); it != children_.end();) {
    if (*it && !(*it)->IsNeedRemove()) {
      (*it)->Update(delta_time, context);
      ++it;
    } else {
      it = children_.erase(it);
    }
  }
}

void UIElement::Render(engine::core::Context& context) {
  if (!visible_) return;

  for (const auto& child : children_) {
    if (child) child->Render(context);
  }
}

void UIElement::AddChild(std::unique_ptr<UIElement> child) {
  if (child) {
    child->SetParent(this);
    children_.push_back(std::move(child));
  }
}

std::unique_ptr<UIElement> UIElement::RemoveChild(UIElement* child_ptr) {
  auto it = std::find_if(children_.begin(), children_.end(),
                         [child_ptr](const std::unique_ptr<UIElement>& p) {
                           return p.get() == child_ptr;
                         });

  if (it == children_.end()) {
    return nullptr;
  }
  std::unique_ptr<UIElement> removed_child = std::move(*it);
  children_.erase(it);
  removed_child->SetParent(nullptr);
  return removed_child;
}

void UIElement::RemoveAllChildren() {
  for (auto& child : children_) {
    child->SetParent(nullptr);
  }
  children_.clear();
}

glm::vec2 UIElement::GetScreenPosition() const {
  if (parent_) {
    return parent_->GetScreenPosition() + position_;
  }
  return position_;
}

engine::utils::Rect UIElement::GetBounds() const {
  auto abs_pos = GetScreenPosition();
  return engine::utils::Rect(abs_pos, size_);
}

bool UIElement::IsPointInside(const glm::vec2& point) const {
  auto bounds = GetBounds();
  return (point.x >= bounds.position.x &&
          point.x < (bounds.position.x + bounds.size.x) &&
          point.y >= bounds.position.y &&
          point.y < (bounds.position.y + bounds.size.y));
}

}  // namespace engine::ui
