// Copyright Sylar129

#pragma once

#include <memory>
#include <vector>

#include "utils/math.h"
#include "utils/non_copyable.h"

namespace engine::core {
class Context;
}

namespace engine::ui {

class UIElement {
 public:
  explicit UIElement(const glm::vec2& position = {0.0f, 0.0f},
                     const glm::vec2& size = {0.0f, 0.0f});

  virtual ~UIElement() = default;

  DISABLE_COPY_AND_MOVE(UIElement);

  virtual bool HandleInput(core::Context& context);
  virtual void Update(float delta_time, core::Context& context);
  virtual void Render(core::Context& context);

  void AddChild(std::unique_ptr<UIElement> child);
  std::unique_ptr<UIElement> RemoveChild(UIElement* child_ptr);
  void RemoveAllChildren();

  const glm::vec2& GetSize() const { return size_; }
  const glm::vec2& GetPosition() const { return position_; }
  bool IsVisible() const { return visible_; }
  bool IsNeedRemove() const { return need_remove_; }
  UIElement* GetParent() const { return parent_; }
  const std::vector<std::unique_ptr<UIElement>>& GetChildren() const {
    return children_;
  }

  void SetSize(const glm::vec2& size) { size_ = size; }
  void SetVisible(bool visible) { visible_ = visible; }
  void SetParent(UIElement* parent) { parent_ = parent; }
  void SetPosition(const glm::vec2& position) { position_ = position; }
  void SetNeedRemove(bool need_remove) { need_remove_ = need_remove; }

  utils::Rect GetBounds() const;
  glm::vec2 GetScreenPosition() const;
  bool IsPointInside(const glm::vec2& point) const;

 protected:
  UIElement* parent_ = nullptr;
  glm::vec2 position_;  // relative to parent
  glm::vec2 size_;
  bool visible_ = true;
  bool need_remove_ = false;

  std::vector<std::unique_ptr<UIElement>> children_;
};

}  // namespace engine::ui
