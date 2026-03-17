// Copyright Sylar129

#include "engine/input/input_manager.h"

#include "SDL3/SDL.h"
#include "engine/core/config.h"
#include "glm/vec2.hpp"
#include "utils/assert.h"
#include "utils/log.h"

namespace engine::input {

InputManager::InputManager(SDL_Renderer* sdl_renderer,
                           const engine::core::Config* config)
    : sdl_renderer_(sdl_renderer) {
  ENGINE_LOG_ASSERT(sdl_renderer_, "InputManager: SDL_Renderer is nullptr");

  InitializeMappings(config);
  SDL_GetMouseState(&mouse_position_.x, &mouse_position_.y);
  ENGINE_LOG_TRACE("Initial mouse position: ({}, {})", mouse_position_.x,
                   mouse_position_.y);
}

void InputManager::Update() {
  for (auto& [action_name, state] : action_states_) {
    if (state == ActionState::PRESSED_THIS_FRAME) {
      state = ActionState::HELD_DOWN;
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
      SDL_Scancode scancode = event.key.scancode;
      bool is_down = event.key.down;
      bool is_repeat = event.key.repeat;

      auto it = scancode_to_actions_map_.find(scancode);
      if (it != scancode_to_actions_map_.end()) {
        const std::vector<std::string>& associated_actions = it->second;
        for (const std::string& action_name : associated_actions) {
          UpdateActionState(action_name, is_down, is_repeat);
        }
      }
      break;
    }
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
    case SDL_EVENT_MOUSE_BUTTON_UP: {
      Uint8 button = event.button.button;
      bool is_down = event.button.down;
      auto it = mouse_button_to_actions_map_.find(button);
      if (it != mouse_button_to_actions_map_.end()) {
        const std::vector<std::string>& associated_actions = it->second;
        for (const std::string& action_name : associated_actions) {
          UpdateActionState(action_name, is_down, false);
        }
      }
      mouse_position_ = {event.button.x, event.button.y};
      break;
    }
    case SDL_EVENT_MOUSE_MOTION:
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
  ENGINE_LOG_TRACE("Initializint input mappings...");

  ENGINE_LOG_ASSERT(config, "InputManager: Config is nullptr");

  actions_to_keyname_map_ = config->input_mappings;
  scancode_to_actions_map_.clear();
  mouse_button_to_actions_map_.clear();
  action_states_.clear();

  if (actions_to_keyname_map_.find("MouseLeftClick") ==
      actions_to_keyname_map_.end()) {
    ENGINE_LOG_DEBUG(
        "No action 'MouseLeftClick' in Config. Adding default mapping "
        "'MouseLeft'.");
    actions_to_keyname_map_["MouseLeftClick"] = {"MouseLeft"};
  }
  if (actions_to_keyname_map_.find("MouseRightClick") ==
      actions_to_keyname_map_.end()) {
    ENGINE_LOG_DEBUG(
        "No action 'MouseRightClick' in Config. Adding default mapping "
        "'MouseRight'.");
    actions_to_keyname_map_["MouseRightClick"] = {"MouseRight"};
  }
  for (const auto& [action_name, key_names] : actions_to_keyname_map_) {
    action_states_[action_name] = ActionState::INACTIVE;
    ENGINE_LOG_TRACE("mapping action: {}", action_name);
    for (const std::string& key_name : key_names) {
      SDL_Scancode scancode = ScancodeFromString(key_name);
      Uint8 mouse_button = MouseButtonUint8FromString(key_name);

      if (scancode != SDL_SCANCODE_UNKNOWN) {
        scancode_to_actions_map_[scancode].push_back(action_name);
        ENGINE_LOG_TRACE("  Mapping keyboard: {} (Scancode: {}) to Action: {}",
                         key_name, static_cast<int>(scancode), action_name);
      } else if (mouse_button != 0) {
        mouse_button_to_actions_map_[mouse_button].push_back(action_name);
        ENGINE_LOG_TRACE(
            "  Mappding mouse button: {} (Button ID: {}) to Action: {}",
            key_name, static_cast<int>(mouse_button), action_name);
      } else {
        ENGINE_LOG_WARN("Unknown key or button: '{}' for action: '{}'.",
                        key_name, action_name);
      }
    }
  }
  ENGINE_LOG_TRACE("Initialize input mappings successfully.");
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
    ENGINE_LOG_WARN("Trying to update unregistered action: {}", action_name);
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
