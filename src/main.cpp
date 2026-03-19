// Copyright Sylar129

#include "engine/core/game_app.h"
#include "utils/log.h"

int main(int /* argc */, char** /* argv */) {
  Log::Init();

  engine::core::GameApp app;
  if (!app.Init()) {
    ENGINE_LOG_CRITICAL("Failed to init GameApp!");
    return -1;
  }
  app.Run();
  app.Close();
  return 0;
}
