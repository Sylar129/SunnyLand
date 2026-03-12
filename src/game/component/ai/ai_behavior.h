// Copyright Sylar129

#pragma once

#include "engine/utils/non_copyable.h"

namespace game::component {
class AIComponent;
}

namespace game::component::ai {

class AIBehavior {
  friend class game::component::AIComponent;

 public:
  AIBehavior() = default;
  virtual ~AIBehavior() = default;

  DISABLE_COPY_AND_MOVE(AIBehavior);

 protected:
  virtual void Enter(AIComponent&) {}
  virtual void Update(float, AIComponent&) = 0;
};

}  // namespace game::component::ai
