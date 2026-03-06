// Copyright Sylar129

#include "engine/scene/level_loader.h"

#include <filesystem>
#include <fstream>

#include "engine/component/parallax_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/tilelayer_component.h"
#include "engine/component/transform_component.h"
#include "engine/core/context.h"
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
  if (!layer_json.contains("objects") || !layer_json["objects"].is_array()) {
    ENGINE_ERROR("对象图层 '{}' 缺少 'objects' 属性。",
                 layer_json.value("name", "Unnamed"));
    return;
  }

  const auto& objects = layer_json["objects"];
  for (const auto& object : objects) {
    auto gid = object.value("gid", 0);

    if (gid == 0) {
      // 如果gid为0，代表是自定义形状，如碰撞盒，我们以后再处理
      // TODO: Handle shapes
    } else {
      // 如果gid存在，则代表这是一个带图像的对象
      auto tile_info = GetTileInfoByGid(gid);
      if (tile_info.sprite.GetTextureId().empty()) {
        ENGINE_ERROR("gid为 {} 的瓦片没有图像纹理。", gid);
        continue;
      }

      // 1. 获取Transform信息
      auto position =
          glm::vec2(object.value("x", 0.0f), object.value("y", 0.0f));
      auto dst_size =
          glm::vec2(object.value("width", 0.0f), object.value("height", 0.0f));

      // !! 关键的坐标转换 !!
      position = glm::vec2(position.x, position.y - dst_size.y);

      auto rotation = object.value("rotation", 0.0f);

      // 2. 计算缩放
      auto src_size_opt = tile_info.sprite.GetSourceRect();
      if (!src_size_opt) {
        ENGINE_ERROR("gid为 {} 的瓦片没有源矩形。", gid);
        continue;
      }
      auto src_size = glm::vec2(src_size_opt->w, src_size_opt->h);
      auto scale = dst_size / src_size;

      // 3. 获取对象名称
      const std::string& object_name = object.value("name", "Unnamed");

      // 4. 创建GameObject并添加组件
      auto game_object =
          std::make_unique<engine::object::GameObject>(object_name);
      game_object->AddComponent<engine::component::TransformComponent>(
          position, scale, rotation);
      game_object->AddComponent<engine::component::SpriteComponent>(
          std::move(tile_info.sprite), scene.GetContext().getResourceManager());

      // 5. 添加到场景中
      scene.AddGameObject(std::move(game_object));
      ENGINE_INFO("加载对象: '{}' 完成", object_name);
    }
  }
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
    if (!tileset.contains(
            "tiles")) {  // 没有tiles字段的话不符合数据格式要求，直接返回空的瓦片信息
      ENGINE_ERROR("Tileset 文件 '{}' 缺少 'tiles' 属性。", tileset_it->first);
      return engine::component::TileInfo();
    }
    // 遍历tiles数组，根据id查找对应的瓦片
    const auto& tiles_json = tileset["tiles"];
    for (const auto& tile_json : tiles_json) {
      auto tile_id = tile_json.value("id", 0);
      if (tile_id == local_id) {  // 找到对应的瓦片，进行后续操作
        if (!tile_json.contains(
                "image")) {  // 没有image字段的话不符合数据格式要求，直接返回空的瓦片信息
          ENGINE_ERROR("Tileset 文件 '{}' 中瓦片 {} 缺少 'image' 属性。",
                       tileset_it->first, tile_id);
          return engine::component::TileInfo();
        }
        // --- 接下来根据必要信息创建并返回 TileInfo ---
        // 获取图片路径
        auto texture_id =
            ResolvePath(tile_json["image"].get<std::string>(), file_path);
        // 先确认图片尺寸
        auto image_width = tile_json.value("imagewidth", 0);
        auto image_height = tile_json.value("imageheight", 0);
        // 从json中获取源矩形信息
        SDL_FRect texture_rect = {
            // tiled中源矩形信息只有设置了才会有值，没有就是默认值
            static_cast<float>(tile_json.value("x", 0)),
            static_cast<float>(tile_json.value("y", 0)),
            static_cast<float>(tile_json.value(
                "width", image_width)),  // 如果未设置，则使用图片尺寸
            static_cast<float>(tile_json.value("height", image_height))};
        engine::render::Sprite sprite{texture_id, texture_rect};
        return engine::component::TileInfo(
            sprite, engine::component::TileType::
                        NORMAL);  // 目前只完成渲染，以后再考虑瓦片类型
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
