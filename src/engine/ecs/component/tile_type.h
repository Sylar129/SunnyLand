// Copyright Sylar129

#pragma once

namespace engine::ecs {

enum class TileType {
  kEmpty,
  kNormal,
  kSolid,
  kUnisolid,
  kSlope0_1,
  kSlope1_0,
  kSlope0_2,
  kSlope2_1,
  kSlope1_2,
  kSlope2_0,
  kHazard,
  kLadder,
};

}  // namespace engine::ecs