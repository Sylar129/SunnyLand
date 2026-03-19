// Copyright Sylar129

#pragma once

#include <map>
#include <string>

#include "engine/ecs/components.h"
#include "entt/entt.hpp"
#include "nlohmann/json.hpp"
#include "utils/math.h"

namespace engine::core {
class Context;
}

namespace engine::scene {

class EcsLevelLoader final {
 public:
  bool LoadLevel(const std::string& map_path, core::Context& context,
                 entt::registry& registry);

 private:
  void LoadImageLayer(const nlohmann::json& layer_json, entt::registry& registry,
                      int layer_order);
  void LoadTileLayer(const nlohmann::json& layer_json, entt::registry& registry,
                     int layer_order);
  void LoadObjectLayer(const nlohmann::json& layer_json, core::Context& context,
                       entt::registry& registry, int layer_order);
  void AddAnimation(const nlohmann::json& anim_json,
                    engine::ecs::AnimationComponent& animation_component,
                    const glm::vec2& sprite_size) const;
  void LoadTileset(const std::string& tileset_path, int first_gid);
  engine::ecs::TileInfo GetTileInfoByGid(int gid) const;
  std::string ResolvePath(const std::string& relative_path,
                          const std::string& file_path) const;
  engine::ecs::TileType GetTileType(const nlohmann::json& tile_json) const;
  engine::ecs::TileType GetTileTypeById(const nlohmann::json& tileset_json,
                                        int local_id) const;

  template <typename T>
  std::optional<T> GetTileProperty(const nlohmann::json& tile_json,
                                   const std::string& property_name) const {
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

  std::optional<utils::Rect> GetColliderRect(
      const nlohmann::json& tile_json) const;
  std::optional<nlohmann::json> GetTileJsonByGid(int gid) const;

  std::string map_path_;
  glm::ivec2 map_size_ = {0, 0};
  glm::ivec2 tile_size_ = {0, 0};
  std::map<int, nlohmann::json> tileset_data_;
};

}  // namespace engine::scene