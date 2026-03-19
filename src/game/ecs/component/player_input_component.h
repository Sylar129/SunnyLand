// Copyright Sylar129

#pragma once

namespace game::ecs {

struct PlayerInputComponent {
  bool move_left = false;
  bool move_right = false;
  bool move_up = false;
  bool move_down = false;
  bool jump_pressed = false;
};

}  // namespace game::ecs