// Copyright Sylar129

#pragma once

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"
#include "glm/vec2.hpp"

namespace engine::core {

enum class State {
  kTitle,
  kPlaying,
  kPaused,
  kGameOver,
};

class GameState final {
 public:
  explicit GameState(SDL_Window* window, SDL_Renderer* renderer,
                     State initial_state = State::kTitle);

  State GetCurrentState() const { return current_state_; }
  void SetState(State new_state);
  glm::vec2 GetWindowSize() const;
  void SetWindowSize(const glm::vec2& new_size);
  glm::vec2 GetLogicalSize() const;
  void SetLogicalSize(const glm::vec2& new_size);

  bool IsInTitle() const { return current_state_ == State::kTitle; }
  bool IsPlaying() const { return current_state_ == State::kPlaying; }
  bool IsPaused() const { return current_state_ == State::kPaused; }
  bool IsGameOver() const { return current_state_ == State::kGameOver; }

 private:
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  State current_state_ = State::kTitle;
};

}  // namespace engine::core
