// Copyright Sylar129

#pragma once

#include <optional>
#include <string>
#include <string_view>

#include "engine/component/tilelayer_component.h"
#include "nlohmann/json.hpp"

namespace game::data {

inline constexpr std::string_view kHazardTileTrigger = "hazard";

template <typename T>
std::optional<T> GetTileProperty(const nlohmann::json& tile_json,
                                 const std::string& property_name) {
  if (!tile_json.contains("properties")) {
    return std::nullopt;
  }

  for (const auto& property : tile_json["properties"]) {
    if (property.value("name", "") == property_name) {
      return property.value("value", T{});
    }
  }

  return std::nullopt;
}

inline bool ApplyLegacySlopeType(const std::string& slope_type,
                                 engine::component::TileInfo& tile_info) {
  using engine::component::TileCollisionType;

  tile_info.physics.collision = TileCollisionType::kSlope;
  if (slope_type == "0_1") {
    tile_info.physics.slope = {0.0f, 1.0f};
  } else if (slope_type == "1_0") {
    tile_info.physics.slope = {1.0f, 0.0f};
  } else if (slope_type == "0_2") {
    tile_info.physics.slope = {0.0f, 0.5f};
  } else if (slope_type == "2_1") {
    tile_info.physics.slope = {0.5f, 1.0f};
  } else if (slope_type == "1_2") {
    tile_info.physics.slope = {1.0f, 0.5f};
  } else if (slope_type == "2_0") {
    tile_info.physics.slope = {0.5f, 0.0f};
  } else {
    return false;
  }

  return true;
}

inline auto CreateSunnyLandTileInfoResolver() {
  return [](const nlohmann::json& tile_json,
            engine::component::TileInfo& tile_info) {
    using engine::component::TileCollisionType;

    if (auto solid = GetTileProperty<bool>(tile_json, "solid");
        solid && solid.value()) {
      tile_info.physics.collision = TileCollisionType::kSolid;
    }

    if (auto one_way = GetTileProperty<bool>(tile_json, "unisolid");
        one_way && one_way.value()) {
      tile_info.physics.collision = TileCollisionType::kOneWay;
    }

    if (auto slope = GetTileProperty<std::string>(tile_json, "slope"); slope) {
      ApplyLegacySlopeType(slope.value(), tile_info);
    }

    if (auto ladder = GetTileProperty<bool>(tile_json, "ladder");
        ladder && ladder.value()) {
      tile_info.physics.is_climbable = true;
    }

    if (auto hazard = GetTileProperty<bool>(tile_json, "hazard");
        hazard && hazard.value()) {
      tile_info.trigger_tag = std::string(kHazardTileTrigger);
    }
  };
}

}  // namespace game::data