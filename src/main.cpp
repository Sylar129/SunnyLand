// Copyright Sylar129

#include "engine/core/game_app.h"
#include "utils/log.h"

int main(int /* argc */, char** /* argv */) {
  Log::Init();
  engine::core::GameApp app;
  app.Run();
  return 0;
}
