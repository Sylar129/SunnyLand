// Copyright Sylar129

#pragma once

#include <map>
#include <string>

#include "glm/vec2.hpp"
#include "nlohmann/json.hpp"

namespace engine::component {
struct TileInfo;
enum class TileType;
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

  void LoadTileset(const std::string& tileset_path, int first_gid);
  engine::component::TileInfo GetTileInfoByGid(int gid) const;
  std::string ResolvePath(const std::string& relative_path,
                          const std::string& file_path) const;

  engine::component::TileType getTileType(
      const nlohmann::json& tile_json) const;
  engine::component::TileType getTileTypeById(
      const nlohmann::json& tileset_json, int local_id) const;

  std::string map_path_;
  glm::ivec2 map_size_;
  glm::ivec2 tile_size_;
  std::map<int, nlohmann::json> tileset_data_;  // firstgid -> tileset json data
};

}  // namespace engine::scene
