// Copyright Sylar129

#include "engine/input/input_manager.h"

#include <stdexcept>

#include "SDL3/SDL.h"
#include "engine/core/config.h"
#include "glm/vec2.hpp"
#include "log.h"

namespace engine::input {

InputManager::InputManager(SDL_Renderer* sdl_renderer,
                           const engine::core::Config* config)
    : sdl_renderer_(sdl_renderer) {
  if (!sdl_renderer_) {
    ENGINE_ERROR("InputManager: SDL_Renderer is nullptr");
    throw std::runtime_error("InputManager: SDL_Renderer is nullptr");
  }
  InitializeMappings(config);
  SDL_GetMouseState(&mouse_position_.x, &mouse_position_.y);
  ENGINE_TRACE("Initial mouse position: ({}, {})", mouse_position_.x,
               mouse_position_.y);
}

void InputManager::Update() {
  for (auto& [action_name, state] : action_states_) {
    if (state == ActionState::PRESSED_THIS_FRAME) {
      state =
          ActionState::HELD_DOWN;  // 当某个键按下不动时，并不会生成SDL_Event。
    } else if (state == ActionState::RELEASED_THIS_FRAME) {
      state = ActionState::INACTIVE;
    }
  }

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ProcessEvent(event);
  }
}

void InputManager::ProcessEvent(const SDL_Event& event) {
  switch (event.type) {
    case SDL_EVENT_KEY_DOWN:
    case SDL_EVENT_KEY_UP: {
      SDL_Scancode scancode = event.key.scancode;  // 获取按键的scancode
      bool is_down = event.key.down;
      bool is_repeat = event.key.repeat;

      auto it = scancode_to_actions_map_.find(scancode);
      if (it != scancode_to_actions_map_.end()) {  // 如果按键有对应的action
        const std::vector<std::string>& associated_actions = it->second;
        for (const std::string& action_name : associated_actions) {
          UpdateActionState(action_name, is_down, is_repeat);  // 更新action状态
        }
      }
      break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
      Uint8 button = event.button.button;  // 获取鼠标按钮
      bool is_down = event.button.down;
      auto it = mouse_button_to_actions_map_.find(button);
      if (it !=
          mouse_button_to_actions_map_.end()) {  // 如果鼠标按钮有对应的action
        const std::vector<std::string>& associated_actions = it->second;
        for (const std::string& action_name : associated_actions) {
          // 鼠标事件不考虑repeat, 所以第三个参数传false
          UpdateActionState(action_name, is_down, false);  // 更新action状态
        }
      }
      // 在点击时更新鼠标位置
      mouse_position_ = {event.button.x, event.button.y};
      break;
    }
    case SDL_EVENT_MOUSE_MOTION:  // 处理鼠标运动
      mouse_position_ = {event.motion.x, event.motion.y};
      break;
    case SDL_EVENT_QUIT:
      should_quit_ = true;
      break;
    default:
      break;
  }
}

bool InputManager::IsActionDown(const std::string& action_name) const {
  if (auto it = action_states_.find(action_name); it != action_states_.end()) {
    return it->second == ActionState::PRESSED_THIS_FRAME ||
           it->second == ActionState::HELD_DOWN;
  }
  return false;
}

bool InputManager::IsActionPressed(const std::string& action_name) const {
  if (auto it = action_states_.find(action_name); it != action_states_.end()) {
    return it->second == ActionState::PRESSED_THIS_FRAME;
  }
  return false;
}

bool InputManager::IsActionReleased(const std::string& action_name) const {
  if (auto it = action_states_.find(action_name); it != action_states_.end()) {
    return it->second == ActionState::RELEASED_THIS_FRAME;
  }
  return false;
}

bool InputManager::ShouldQuit() const { return should_quit_; }

void InputManager::SetShouldQuit(bool should_quit) {
  should_quit_ = should_quit;
}

glm::vec2 InputManager::GetMousePosition() const { return mouse_position_; }

glm::vec2 InputManager::GetLogicalMousePosition() const {
  glm::vec2 logical_pos;
  SDL_RenderCoordinatesFromWindow(sdl_renderer_, mouse_position_.x,
                                  mouse_position_.y, &logical_pos.x,
                                  &logical_pos.y);
  return logical_pos;
}

void InputManager::InitializeMappings(const engine::core::Config* config) {
  ENGINE_TRACE("初始化输入映射...");
  if (!config) {
    ENGINE_ERROR("输入管理器: Config 为空指针");
    throw std::runtime_error("输入管理器: Config 为空指针");
  }
  actions_to_keyname_map_ =
      config->input_mappings_;  // 获取配置中的输入映射（动作 -> 按键名称）
  scancode_to_actions_map_.clear();
  mouse_button_to_actions_map_.clear();
  action_states_.clear();

  // 如果配置中没有定义鼠标按钮动作(通常不需要配置),则添加默认映射, 用于 UI
  if (actions_to_keyname_map_.find("MouseLeftClick") ==
      actions_to_keyname_map_.end()) {
    ENGINE_DEBUG(
        "配置中没有定义 'MouseLeftClick' 动作,添加默认映射到 'MouseLeft'.");
    actions_to_keyname_map_["MouseLeftClick"] = {"MouseLeft"};
  }
  if (actions_to_keyname_map_.find("MouseRightClick") ==
      actions_to_keyname_map_.end()) {
    ENGINE_DEBUG(
        "配置中没有定义 'MouseRightClick' 动作,添加默认映射到 'MouseRight'.");
    actions_to_keyname_map_["MouseRightClick"] = {"MouseRight"};
  }
  // 遍历 动作 -> 按键名称 的映射
  for (const auto& [action_name, key_names] : actions_to_keyname_map_) {
    action_states_[action_name] = ActionState::INACTIVE;
    ENGINE_TRACE("映射动作: {}", action_name);
    // 设置 "按键 -> 动作" 的映射
    for (const std::string& key_name : key_names) {
      SDL_Scancode scancode =
          ScancodeFromString(key_name);  // 尝试根据按键名称获取scancode
      Uint8 mouse_button =
          MouseButtonUint8FromString(key_name);  // 尝试根据按键名称获取鼠标按钮
      // 未来可添加其它输入类型 ...

      if (scancode !=
          SDL_SCANCODE_UNKNOWN) {  // 如果scancode有效,则将action添加到scancode_to_actions_map_中
        scancode_to_actions_map_[scancode].push_back(action_name);
        ENGINE_TRACE("  映射按键: {} (Scancode: {}) 到动作: {}", key_name,
                     static_cast<int>(scancode), action_name);
      } else if (
          mouse_button !=
          0) {  // 如果鼠标按钮有效,则将action添加到mouse_button_to_actions_map_中
        mouse_button_to_actions_map_[mouse_button].push_back(action_name);
        ENGINE_TRACE("  映射鼠标按钮: {} (Button ID: {}) 到动作: {}", key_name,
                     static_cast<int>(mouse_button), action_name);
      } else {
        ENGINE_WARN("输入映射警告: 未知键或按钮名称 '{}' 用于动作 '{}'.",
                    key_name, action_name);
      }
    }
  }
  ENGINE_TRACE("Initialize input mappings successfully.");
}

SDL_Scancode InputManager::ScancodeFromString(const std::string& key_name) {
  return SDL_GetScancodeFromName(key_name.c_str());
}

Uint8 InputManager::MouseButtonUint8FromString(const std::string& button_name) {
  if (button_name == "MouseLeft") return SDL_BUTTON_LEFT;
  if (button_name == "MouseMiddle") return SDL_BUTTON_MIDDLE;
  if (button_name == "MouseRight") return SDL_BUTTON_RIGHT;
  if (button_name == "MouseX1") return SDL_BUTTON_X1;
  if (button_name == "MouseX2") return SDL_BUTTON_X2;
  return 0;
}

void InputManager::UpdateActionState(const std::string& action_name,
                                     bool is_input_active,
                                     bool is_repeat_event) {
  auto it = action_states_.find(action_name);
  if (it == action_states_.end()) {
    ENGINE_WARN("Trying to update unregistered action: {}", action_name);
    return;
  }

  if (is_input_active) {
    if (is_repeat_event) {
      it->second = ActionState::HELD_DOWN;
    } else {
      it->second = ActionState::PRESSED_THIS_FRAME;
    }
  } else {
    it->second = ActionState::RELEASED_THIS_FRAME;
  }
}

}  // namespace engine::input