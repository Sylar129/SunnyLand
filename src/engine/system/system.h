// Copyright Sylar129

#pragma once

#include "utils/non_copyable.h"

namespace engine::system {

class System {
 public:
  System() = default;
  virtual ~System() = default;
  DISABLE_COPY_AND_MOVE(System);

  virtual void Update(float delta_time) = 0;
};

}
