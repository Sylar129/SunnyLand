// Copyright Sylar129

#pragma once

#include <map>
#include <string>

#include "engine/utils/math.h"
#include "glm/vec2.hpp"
#include "nlohmann/json.hpp"

namespace engine::component {
struct TileInfo;
enum class TileType;
class AnimationComponent;
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

  void addAnimation(const nlohmann::json& anim_json,
                    engine::component::AnimationComponent* ac,
                    const glm::vec2& sprite_size);

  void LoadTileset(const std::string& tileset_path, int first_gid);
  engine::component::TileInfo GetTileInfoByGid(int gid) const;
  std::string ResolvePath(const std::string& relative_path,
                          const std::string& file_path) const;

  engine::component::TileType GetTileType(
      const nlohmann::json& tile_json) const;
  engine::component::TileType GetTileTypeById(
      const nlohmann::json& tileset_json, int local_id) const;

  template <typename T>
  std::optional<T> GetTileProperty(const nlohmann::json& tile_json,
                                   const std::string& property_name) const {
    if (!tile_json.contains("properties")) return std::nullopt;
    const auto& properties = tile_json["properties"];
    for (const auto& property : properties) {
      if (property.value("name", "") == property_name) {
        return property.value("value", T{});
      }
    }
    return std::nullopt;
  }

  std::optional<engine::utils::Rect> GetColliderRect(
      const nlohmann::json& tile_json) const;

  std::optional<nlohmann::json> GetTileJsonByGid(int gid) const;

  std::string map_path_;
  glm::ivec2 map_size_;
  glm::ivec2 tile_size_;
  std::map<int, nlohmann::json> tileset_data_;  // firstgid -> tileset json data
};

}  // namespace engine::scene
