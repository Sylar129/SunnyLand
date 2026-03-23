// Copyright Sylar129

#pragma once

#include <memory>
#include <string>

#include "engine/resource/tiled_types.h"

namespace engine::resource {

class TiledParser final {
 public:
  std::shared_ptr<TiledMap> LoadMap(const std::string& file_path) const;
};

}  // namespace engine::resource