// Copyright Sylar129

#pragma once

#include <functional>
#include <map>
#include <optional>
#include <string>
#include <utility>

#include "glm/vec2.hpp"
#include "nlohmann/json.hpp"
#include "utils/math.h"

namespace engine::component {
struct TileInfo;
struct TilePhysics;
class AnimationComponent;
}  // namespace engine::component

namespace engine::render {
class Sprite;
}

namespace engine::scene {

using TileInfoResolver =
    std::function<void(const nlohmann::json&, component::TileInfo&)>;

class Scene;

class LevelLoader final {
 public:
  explicit LevelLoader(TileInfoResolver resolver = {})
      : tile_info_resolver_(std::move(resolver)) {}

  bool LoadLevel(const std::string& map_path, Scene& scene);
  void SetTileInfoResolver(TileInfoResolver resolver) {
    tile_info_resolver_ = std::move(resolver);
  }

 private:
  void LoadImageLayer(const nlohmann::json& layer_json, Scene& scene);
  void LoadTileLayer(const nlohmann::json& layer_json, Scene& scene);
  void LoadObjectLayer(const nlohmann::json& layer_json, Scene& scene);

  void AddAnimation(const nlohmann::json& anim_json,
                    component::AnimationComponent* ac,
                    const glm::vec2& sprite_size);

  void LoadTileset(const std::string& tileset_path, int first_gid);
  component::TileInfo GetTileInfoByGid(int gid) const;
  component::TileInfo ResolveTileInfo(render::Sprite sprite,
                                      const nlohmann::json* tile_json) const;
  std::string ResolvePath(const std::string& relative_path,
                          const std::string& file_path) const;

  component::TilePhysics GetTilePhysics(
      const nlohmann::json& tile_json) const;
  bool ParseSlopeHeights(const std::string& slope_heights,
                         component::TilePhysics& tile_physics) const;

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

  std::optional<utils::Rect> GetColliderRect(
      const nlohmann::json& tile_json) const;

  std::optional<nlohmann::json> GetTileJsonByGid(int gid) const;

  std::string map_path_;
  glm::ivec2 map_size_;
  glm::ivec2 tile_size_;
  std::map<int, nlohmann::json> tileset_data_;  // firstgid -> tileset json data
  TileInfoResolver tile_info_resolver_;
};

}  // namespace engine::scene
