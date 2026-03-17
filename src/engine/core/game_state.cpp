// Copyright Sylar129

#include "engine/core/game_state.h"

#include "engine/utils/assert.h"
#include "log.h"

namespace engine::core {

GameState::GameState(SDL_Window* window, SDL_Renderer* renderer,
                     State initial_state)
    : window_(window), renderer_(renderer), current_state_(initial_state) {
  ENGINE_LOG_ASSERT(window_ && renderer_,
                    "window and renderer must not be null");
  ENGINE_LOG_TRACE("GameState initialized with state: {}",
                   static_cast<int>(current_state_));
}

void GameState::SetState(State new_state) {
  if (current_state_ != new_state) {
    ENGINE_LOG_DEBUG("Changing game state from {} to {}",
                     static_cast<int>(current_state_),
                     static_cast<int>(new_state));
    current_state_ = new_state;
  } else {
    ENGINE_LOG_DEBUG("SetState called with the same state: {}, no change made.",
                     static_cast<int>(new_state));
  }
}

glm::vec2 GameState::GetWindowSize() const {
  int width, height;
  SDL_GetWindowSize(window_, &width, &height);
  return glm::vec2(width, height);
}

void GameState::SetWindowSize(const glm::vec2& new_size) {
  SDL_SetWindowSize(window_, static_cast<int>(new_size.x),
                    static_cast<int>(new_size.y));
}

glm::vec2 GameState::GetLogicalSize() const {
  int width, height;
  SDL_GetRenderLogicalPresentation(renderer_, &width, &height, nullptr);
  return glm::vec2(width, height);
}

void GameState::SetLogicalSize(const glm::vec2& new_size) {
  SDL_SetRenderLogicalPresentation(renderer_, static_cast<int>(new_size.x),
                                   static_cast<int>(new_size.y),
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);
  ENGINE_LOG_TRACE("SetLogicalSize: {}x{}", new_size.x, new_size.y);
}

}  // namespace engine::core
