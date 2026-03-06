// Copyright Sylar129

#include "engine/scene/level_loader.h"

#include <filesystem>
#include <fstream>

#include "engine/component/parallax_component.h"
#include "engine/component/tilelayer_component.h"
#include "engine/component/transform_component.h"
#include "engine/object/game_object.h"
#include "engine/scene/scene.h"
#include "log.h"
#include "nlohmann/json.hpp"

namespace engine::scene {

bool LevelLoader::LoadLevel(const std::string& level_path, Scene& scene) {
  map_path_ = level_path;

  std::ifstream file(level_path);
  if (!file.is_open()) {
    ENGINE_ERROR("Can't open level file: {}", level_path);
    return false;
  }

  nlohmann::json json_data;
  try {
    file >> json_data;
  } catch (const nlohmann::json::parse_error& e) {
    ENGINE_ERROR("Parsing JSON failed: {}", e.what());
    return false;
  }

  map_size_ =
      glm::ivec2(json_data.value("width", 0), json_data.value("height", 0));
  tile_size_ = glm::ivec2(json_data.value("tilewidth", 0),
                          json_data.value("tileheight", 0));

  if (json_data.contains("tilesets") && json_data["tilesets"].is_array()) {
    for (const auto& tileset_json : json_data["tilesets"]) {
      auto tileset_path = ResolvePath(tileset_json["source"], map_path_);
      auto first_gid = tileset_json["firstgid"];
      LoadTileset(tileset_path, first_gid);
    }
  }

  if (!json_data.contains("layers") || !json_data["layers"].is_array()) {
    ENGINE_ERROR("Missing or invalid 'layers' in map '{}'", level_path);
    return false;
  }
  for (const auto& layer_json : json_data["layers"]) {
    std::string layer_type = layer_json.value("type", "none");
    if (!layer_json.value("visible", true)) {
      ENGINE_INFO("Layer '{}' is invisible. Skipping.",
                  layer_json.value("name", "Unnamed"));
      continue;
    }

    if (layer_type == "imagelayer") {
      LoadImageLayer(layer_json, scene);
    } else if (layer_type == "tilelayer") {
      LoadTileLayer(layer_json, scene);
    } else if (layer_type == "objectgroup") {
      LoadObjectLayer(layer_json, scene);
    } else {
      ENGINE_WARN("Unsupported layer type: {}", layer_type);
    }
  }

  ENGINE_INFO("Level [{}] loaded.", level_path);
  return true;
}

void LevelLoader::LoadImageLayer(const nlohmann::json& layer_json,
                                 Scene& scene) {
  const std::string& image_path = layer_json.value("image", "");
  if (image_path.empty()) {
    ENGINE_ERROR("Missing 'image' in Layer '{}'",
                 layer_json.value("name", "Unnamed"));
    return;
  }
  auto texture_id = ResolvePath(image_path, map_path_);

  const glm::vec2 offset = glm::vec2(layer_json.value("offsetx", 0.0f),
                                     layer_json.value("offsety", 0.0f));

  const glm::vec2 scroll_factor = glm::vec2(
      layer_json.value("parallaxx", 1.0f), layer_json.value("parallaxy", 1.0f));
  const glm::bvec2 repeat = glm::bvec2(layer_json.value("repeatx", false),
                                       layer_json.value("repeaty", false));

  const std::string& layer_name = layer_json.value("name", "Unnamed");

  auto game_object = std::make_unique<engine::object::GameObject>(layer_name);

  game_object->AddComponent<engine::component::TransformComponent>(offset);
  game_object->AddComponent<engine::component::ParallaxComponent>(
      texture_id, scroll_factor, repeat);

  scene.AddGameObject(std::move(game_object));
  ENGINE_INFO("Loading Layer '{}' completed.", layer_name);
}

void LevelLoader::LoadTileLayer(const nlohmann::json& layer_json,
                                Scene& scene) {
  std::vector<engine::component::TileInfo> tiles;
  tiles.reserve(map_size_.x * map_size_.y);

  const auto& data = layer_json["data"];

  for (const auto& gid : data) {
    tiles.push_back(GetTileInfoByGid(gid));
  }

  auto game_object = std::make_unique<engine::object::GameObject>(
      layer_json.value("name", "Unnamed"));
  game_object->AddComponent<engine::component::TileLayerComponent>(
      tile_size_, map_size_, std::move(tiles));
  scene.AddGameObject(std::move(game_object));
}

void LevelLoader::LoadObjectLayer(const nlohmann::json& layer_json,
                                  Scene& scene) {
  // TODO
}

void LevelLoader::LoadTileset(const std::string& tileset_path, int first_gid) {
  std::ifstream tileset_file(tileset_path);
  if (!tileset_file.is_open()) {
    return;
  }

  nlohmann::json ts_json;
  tileset_file >> ts_json;

  ts_json["file_path"] = tileset_path;
  tileset_data_[first_gid] = std::move(ts_json);
}

engine::component::TileInfo LevelLoader::GetTileInfoByGid(int gid) const {
  if (gid == 0) {
    return {};
  }

  auto tileset_it = tileset_data_.upper_bound(gid);
  if (tileset_it == tileset_data_.begin()) {
    return {};
  }
  --tileset_it;

  const auto& tileset = tileset_it->second;
  auto local_id = gid - tileset_it->first;
  const std::string file_path = tileset.value("file_path", "");

  if (tileset.contains("image")) {
    auto texture_id = ResolvePath(tileset["image"], file_path);
    auto columns = tileset.value("columns", 1);
    auto coord_x = local_id % columns;
    auto coord_y = local_id / columns;
    SDL_FRect texture_rect = {static_cast<float>(coord_x * tile_size_.x),
                              static_cast<float>(coord_y * tile_size_.y),
                              static_cast<float>(tile_size_.x),
                              static_cast<float>(tile_size_.y)};
    return {{texture_id, texture_rect}, engine::component::TileType::NORMAL};
  } else {
    const auto& tiles_json = tileset["tiles"];
    for (const auto& tile_json : tiles_json) {
      if (tile_json.value("id", -1) == local_id) {
        auto texture_id = ResolvePath(tile_json["image"], file_path);
        return {{texture_id}, engine::component::TileType::NORMAL};
      }
    }
  }
  return {};
}

std::string LevelLoader::ResolvePath(const std::string& relative_path,
                                     const std::string& file_path) const {
  auto map_dir = std::filesystem::path(file_path).parent_path();
  auto final_path = std::filesystem::canonical(map_dir / relative_path);
  return final_path.string();
}

}  // namespace engine::scene
