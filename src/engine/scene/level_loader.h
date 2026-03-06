// Copyright Sylar129

#pragma once

#include <string>

#include "nlohmann/json_fwd.hpp"

namespace engine::scene {
class Scene;

class LevelLoader final {
 public:
  LevelLoader() = default;

  bool LoadLevel(const std::string& map_path, Scene& scene);

 private:
  void LoadImageLayer(const nlohmann::json& layer_json, Scene& scene);
  void LoadTileLayer(const nlohmann::json& layer_json, Scene& scene);
  void LoadObjectLayer(const nlohmann::json& layer_json, Scene& scene);

  std::string ResolvePath(const std::string& image_path);

  std::string map_path_;
};

}  // namespace engine::scene
