// Copyright Sylar129

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "SDL3/SDL_render.h"
#include "glm/vec2.hpp"

namespace engine::core {
class Config;
}

namespace engine::input {

enum class ActionState {
  INACTIVE,
  PRESSED_THIS_FRAME,
  HELD_DOWN,
  RELEASED_THIS_FRAME
};

class InputManager final {
 private:
  SDL_Renderer* sdl_renderer_;
  std::unordered_map<std::string, std::vector<std::string>>
      actions_to_keyname_map_;  ///< @brief 存储动作名称到按键名称列表的映射
  std::unordered_map<SDL_Scancode, std::vector<std::string>>
      scancode_to_actions_map_;  ///< @brief
                                 ///< 从键盘（Scancode）到关联的动作名称列表
  std::unordered_map<Uint8, std::vector<std::string>>
      mouse_button_to_actions_map_;  ///< @brief 从鼠标按钮 (Uint8)
                                     ///< 到关联的动作名称列表

  std::unordered_map<std::string, ActionState>
      action_states_;  ///< @brief 存储每个动作的当前状态

  bool should_quit_ = false;
  glm::vec2 mouse_position_;  // In Screen

 public:
  InputManager(SDL_Renderer* sdl_renderer, const engine::core::Config* config);

  void Update();

  bool IsActionDown(const std::string& action_name) const;
  bool IsActionPressed(const std::string& action_name) const;
  bool IsActionReleased(const std::string& action_name) const;

  bool ShouldQuit() const;
  void SetShouldQuit(bool should_quit);

  glm::vec2 GetMousePosition() const;
  glm::vec2 GetLogicalMousePosition() const;

 private:
  void ProcessEvent(const SDL_Event& event);
  void InitializeMappings(const engine::core::Config* config);

  void UpdateActionState(const std::string& action_name, bool is_input_active,
                         bool is_repeat_event);
  SDL_Scancode ScancodeFromString(const std::string& key_name);
  Uint8 MouseButtonUint8FromString(const std::string& button_name);
};

}  // namespace engine::input