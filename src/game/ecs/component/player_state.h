// Copyright Sylar129

#pragma once

namespace game::ecs {

enum class PlayerState {
  kIdle,
  kWalk,
  kJump,
  kFall,
  kClimb,
  kHurt,
  kDead,
};

}  // namespace game::ecs