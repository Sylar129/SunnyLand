// Copyright Sylar129

#pragma once

#include <string>
#include <vector>

#include "entt/entity/fwd.hpp"
#include "game/ecs/component/effect_request.h"

namespace game::ecs {

struct RuleEvents {
  int score_delta = 0;
  int heal_amount = 0;
  int player_damage = 0;
  bool request_win = false;
  std::string next_level_name;
  std::vector<entt::entity> destroy_entities;
  std::vector<EffectRequest> effects;
};

}  // namespace game::ecs