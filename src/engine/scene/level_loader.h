// Copyright Sylar129

#pragma once

#include <map>
#include <string>

#include "glm/vec2.hpp"
#include "nlohmann/json.hpp"

namespace engine::component {
struct TileInfo;
}

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

  std::string ResolvePath(const std::string& relative_path,
                          const std::string& file_path);
  engine::component::TileInfo getTileInfoByGid(int gid);
  void loadTileset(const std::string& tileset_path, int first_gid);

  std::string map_path_;
  glm::ivec2 map_size_;
  glm::ivec2 tile_size_;
  std::map<int, nlohmann::json> tileset_data_;  // firstgid -> tileset json data
};

}  // namespace engine::scene
