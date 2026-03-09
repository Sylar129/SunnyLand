// Copyright Sylar129

#include "engine/scene/level_loader.h"

#include <filesystem>
#include <fstream>

#include "engine/component/collider_component.h"
#include "engine/component/parallax_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/tilelayer_component.h"
#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/object/game_object.h"
#include "engine/physics/collider.h"
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
    ENGINE_ERROR("Object layer '{}' missing 'objects' attribute.",
                 layer_json.value("name", "Unnamed"));
    return;
  }

  const auto& objects = layer_json["objects"];
  for (const auto& object : objects) {
    auto gid = object.value("gid", 0);

    if (gid == 0) {
      // TODO: Handle shapes
    } else {
      auto tile_info = GetTileInfoByGid(gid);
      if (tile_info.sprite.GetTextureId().empty()) {
        ENGINE_ERROR("Tile {} does not have texture.", gid);
        continue;
      }

      auto position =
          glm::vec2(object.value("x", 0.0f), object.value("y", 0.0f));
      auto dst_size =
          glm::vec2(object.value("width", 0.0f), object.value("height", 0.0f));

      position = glm::vec2(position.x, position.y - dst_size.y);

      auto rotation = object.value("rotation", 0.0f);

      auto src_size_opt = tile_info.sprite.GetSourceRect();
      if (!src_size_opt) {
        ENGINE_ERROR("Tile {} does not have source rect.", gid);
        continue;
      }
      auto src_size = glm::vec2(src_size_opt->w, src_size_opt->h);
      auto scale = dst_size / src_size;

      const std::string& object_name = object.value("name", "Unnamed");

      auto game_object =
          std::make_unique<engine::object::GameObject>(object_name);
      game_object->AddComponent<engine::component::TransformComponent>(
          position, scale, rotation);
      game_object->AddComponent<engine::component::SpriteComponent>(
          std::move(tile_info.sprite), scene.GetContext().GetResourceManager());

      // 获取瓦片json信息      1.
      // 必然存在，因为getTileInfoByGid(gid)函数已经顺利执行
      // 2. 这里再获取json，实际上检索了两次，未来可以优化
      auto tile_json = getTileJsonByGid(gid);

      // 获取碰信息：如果是SOLID类型，则添加物理组件，且图片源矩形区域就是碰撞盒大小
      if (tile_info.type == engine::component::TileType::SOLID) {
        auto collider =
            std::make_unique<engine::physics::AABBCollider>(src_size);
        game_object->AddComponent<engine::component::ColliderComponent>(
            std::move(collider));
        // 物理组件不受重力影响
        game_object->AddComponent<engine::component::PhysicsComponent>(
            &scene.GetContext().GetPhysicsEngine(), false);
        // 设置标签方便物理引擎检索
        game_object->SetTag("solid");
      }
      // 如果非SOLID类型，检查自定义碰撞盒是否存在
      else if (auto rect = getColliderRect(tile_json); rect) {
        // 如果有，添加碰撞组件
        auto collider =
            std::make_unique<engine::physics::AABBCollider>(rect->size);
        auto* cc =
            game_object->AddComponent<engine::component::ColliderComponent>(
                std::move(collider));
        cc->SetOffset(
            rect->position);  // 自定义碰撞盒的坐标是相对于图片坐标，也就是针对Transform的偏移量
        // 和物理组件（默认不受重力影响）
        game_object->AddComponent<engine::component::PhysicsComponent>(
            &scene.GetContext().GetPhysicsEngine(), false);
      }

      // 获取标签信息并设置
      auto tag = getTileProperty<std::string>(tile_json, "tag");
      if (tag) {
        game_object->SetTag(tag.value());
      }

      // 获取重力信息并设置
      auto gravity = getTileProperty<bool>(tile_json, "gravity");
      if (gravity) {
        auto pc =
            game_object->GetComponent<engine::component::PhysicsComponent>();
        if (pc) {
          pc->SetUseGravity(gravity.value());
        } else {
          ENGINE_WARN(
              "对象 '{}' 在设置重力信息时没有物理组件，请检查地图设置。",
              object_name);
          game_object->AddComponent<engine::component::PhysicsComponent>(
              &scene.GetContext().GetPhysicsEngine(), gravity.value());
        }
      }

      scene.AddGameObject(std::move(game_object));
      ENGINE_INFO("Load object: '{}' completed.", object_name);
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
    auto tile_type = GetTileTypeById(tileset, local_id);
    return {{texture_id, texture_rect}, tile_type};
  } else {
    if (!tileset.contains("tiles")) {
      ENGINE_ERROR("Tileset '{}' missing 'tiles' attribute.",
                   tileset_it->first);
      return engine::component::TileInfo();
    }

    const auto& tiles_json = tileset["tiles"];
    for (const auto& tile_json : tiles_json) {
      auto tile_id = tile_json.value("id", 0);
      if (tile_id == local_id) {
        if (!tile_json.contains("image")) {
          ENGINE_ERROR("Tileset '{}' missing 'image' attribute.",
                       tileset_it->first, tile_id);
          return engine::component::TileInfo();
        }

        auto texture_id =
            ResolvePath(tile_json["image"].get<std::string>(), file_path);
        auto image_width = tile_json.value("imagewidth", 0);
        auto image_height = tile_json.value("imageheight", 0);
        SDL_FRect texture_rect = {
            static_cast<float>(tile_json.value("x", 0)),
            static_cast<float>(tile_json.value("y", 0)),
            static_cast<float>(tile_json.value("width", image_width)),
            static_cast<float>(tile_json.value("height", image_height))};
        engine::render::Sprite sprite{texture_id, texture_rect};
        auto tile_type = GetTileType(tile_json);
        return engine::component::TileInfo(sprite, tile_type);
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

engine::component::TileType LevelLoader::GetTileType(
    const nlohmann::json& tile_json) const {
  if (tile_json.contains("properties")) {
    for (const auto& property : tile_json["properties"]) {
      if (property.value("name", "") == "solid") {
        return property.value("value", false)
                   ? engine::component::TileType::SOLID
                   : engine::component::TileType::NORMAL;
      }
    }
  }
  return engine::component::TileType::NORMAL;
}

engine::component::TileType LevelLoader::GetTileTypeById(
    const nlohmann::json& tileset_json, int local_id) const {
  if (tileset_json.contains("tiles")) {
    for (const auto& tile : tileset_json["tiles"]) {
      if (tile.value("id", -1) == local_id) {
        return GetTileType(tile);
      }
    }
  }
  return engine::component::TileType::NORMAL;
}

std::optional<engine::utils::Rect> LevelLoader::getColliderRect(
    const nlohmann::json& tile_json) {
  if (!tile_json.contains("objectgroup")) return std::nullopt;
  auto& objectgroup = tile_json["objectgroup"];
  if (!objectgroup.contains("objects")) return std::nullopt;
  auto& objects = objectgroup["objects"];
  for (const auto& object :
       objects) {  // 一个图片只支持一个碰撞器。如果有多个，则返回第一个不为空的
    auto rect = engine::utils::Rect(
        glm::vec2(object.value("x", 0.0f), object.value("y", 0.0f)),
        glm::vec2(object.value("width", 0.0f), object.value("height", 0.0f)));
    if (rect.size.x > 0 && rect.size.y > 0) {
      return rect;
    }
  }
  return std::nullopt;  // 如果没找到碰撞器，则返回空
}

std::optional<nlohmann::json> LevelLoader::getTileJsonByGid(int gid) const {
  // 1. 查找tileset_data_中键小于等于gid的最近元素
  auto tileset_it = tileset_data_.upper_bound(gid);
  if (tileset_it == tileset_data_.begin()) {
    ENGINE_ERROR("gid为 {} 的瓦片未找到图块集。", gid);
    return std::nullopt;
  }
  --tileset_it;
  // 2. 获取图块集json对象
  const auto& tileset = tileset_it->second;
  auto local_id = gid - tileset_it->first;  // 计算瓦片在图块集中的局部ID
  if (!tileset.contains(
          "tiles")) {  // 没有tiles字段的话不符合数据格式要求，直接返回空
    ENGINE_ERROR("Tileset 文件 '{}' 缺少 'tiles' 属性。", tileset_it->first);
    return std::nullopt;
  }
  // 3. 遍历tiles数组，根据id查找对应的瓦片并返回瓦片json
  const auto& tiles_json = tileset["tiles"];
  for (const auto& tile_json : tiles_json) {
    auto tile_id = tile_json.value("id", 0);
    if (tile_id == local_id) {  // 找到对应的瓦片，返回瓦片json
      return tile_json;
    }
  }
  return std::nullopt;
}

}  // namespace engine::scene
