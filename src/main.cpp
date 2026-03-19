// Copyright Sylar129

#include "engine/core/game_app.h"
#include "entt/entt.hpp"
#include "utils/log.h"

struct Position {
  float x, y;
};

struct Velocity {
  float dx, dy;
};

static void TestEnTT() {
  entt::registry registry;

  // Create entities with components
  const auto e0 = registry.create();
  registry.emplace<Position>(e0, 0.0f, 0.0f);
  registry.emplace<Velocity>(e0, 1.0f, 2.0f);

  const auto e1 = registry.create();
  registry.emplace<Position>(e1, 10.0f, 20.0f);

  // Iterate over entities that have both Position and Velocity
  auto view = registry.view<Position, Velocity>();
  view.each([](auto entity, Position& pos, Velocity& vel) {
    pos.x += vel.dx;
    pos.y += vel.dy;
    ENGINE_LOG_INFO("EnTT entity={} pos=({}, {})",
                    static_cast<uint32_t>(entity), pos.x, pos.y);
  });

  ENGINE_LOG_INFO("EnTT test passed");
}

int main(int /* argc */, char** /* argv */) {
  Log::Init();

  TestEnTT();

  engine::core::GameApp app;
  if (!app.Init()) {
    ENGINE_LOG_CRITICAL("Failed to init GameApp!");
    return -1;
  }
  app.Run();
  app.Close();
  return 0;
}
