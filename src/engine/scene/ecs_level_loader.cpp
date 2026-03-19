// Copyright Sylar129

#include "engine/scene/ecs_level_loader.h"

#include <filesystem>
#include <fstream>
#include <system_error>

#include "engine/core/context.h"
#include "utils/log.h"

namespace engine::scene {

bool EcsLevelLoader::LoadLevel(const std::string& map_path,
                               core::Context& context,
                               entt::registry& registry) {
  map_path_ = map_path;
  std::ifstream file(map_path);
  if (!file.is_open()) {
    ENGINE_LOG_ERROR("Can't open level file: {}", map_path);
    return false;
  }

  const auto json_data = nlohmann::json::parse(file, nullptr, false);
  if (json_data.is_discarded()) {
    ENGINE_LOG_ERROR("Parsing JSON failed for level: {}", map_path);
    return false;
  }

  map_size_ =
      glm::ivec2(json_data.value("width", 0), json_data.value("height", 0));
  tile_size_ = glm::ivec2(json_data.value("tilewidth", 0),
                          json_data.value("tileheight", 0));

  if (json_data.contains("tilesets") && json_data["tilesets"].is_array()) {
    for (const auto& tileset_json : json_data["tilesets"]) {
      const auto tileset_path =
          ResolvePath(tileset_json.value("source", ""), map_path_);
      LoadTileset(tileset_path, tileset_json.value("firstgid", 0));
    }
  }

  if (!json_data.contains("layers") || !json_data["layers"].is_array()) {
    ENGINE_LOG_ERROR("Missing or invalid layers in map '{}'", map_path);
    return false;
  }

  int layer_order = 0;
  for (const auto& layer_json : json_data["layers"]) {
    if (!layer_json.value("visible", true)) {
      continue;
    }
    const std::string layer_type = layer_json.value("type", "");
    if (layer_type == "imagelayer") {
      LoadImageLayer(layer_json, registry, layer_order++);
    } else if (layer_type == "tilelayer") {
      LoadTileLayer(layer_json, registry, layer_order++);
    } else if (layer_type == "objectgroup") {
      LoadObjectLayer(layer_json, context, registry, layer_order++);
    }
  }

  return true;
}

void EcsLevelLoader::LoadImageLayer(const nlohmann::json& layer_json,
                                    entt::registry& registry,
                                    int layer_order) {
  const std::string image_path = layer_json.value("image", "");
  if (image_path.empty()) {
    return;
  }
  const auto entity = registry.create();
  registry.emplace<engine::ecs::NameComponent>(
      entity, layer_json.value("name", "Unnamed"));
  registry.emplace<engine::ecs::RenderOrderComponent>(entity, layer_order);
  registry.emplace<engine::ecs::TransformComponent>(
      entity, glm::vec2(layer_json.value("offsetx", 0.0f),
                        layer_json.value("offsety", 0.0f)),
      glm::vec2(1.0f, 1.0f), 0.0f);
  engine::ecs::ParallaxComponent parallax;
  parallax.sprite.SetTextureId(ResolvePath(image_path, map_path_));
  parallax.scroll_factor =
      glm::vec2(layer_json.value("parallaxx", 1.0f),
                layer_json.value("parallaxy", 1.0f));
  parallax.repeat = glm::bvec2(layer_json.value("repeatx", false),
                               layer_json.value("repeaty", false));
  registry.emplace<engine::ecs::ParallaxComponent>(entity, std::move(parallax));
}

void EcsLevelLoader::LoadTileLayer(const nlohmann::json& layer_json,
                                   entt::registry& registry,
                                   int layer_order) {
  std::vector<engine::ecs::TileInfo> tiles;
  tiles.reserve(static_cast<size_t>(map_size_.x * map_size_.y));
  for (const auto& gid : layer_json["data"]) {
    tiles.push_back(GetTileInfoByGid(gid.get<int>()));
  }
  const auto entity = registry.create();
  registry.emplace<engine::ecs::NameComponent>(
      entity, layer_json.value("name", "Unnamed"));
  registry.emplace<engine::ecs::RenderOrderComponent>(entity, layer_order);
  registry.emplace<engine::ecs::TileLayerComponent>(entity, tile_size_, map_size_,
                                                    std::move(tiles),
                                                    glm::vec2(0.0f, 0.0f), false);
}

void EcsLevelLoader::LoadObjectLayer(const nlohmann::json& layer_json,
                                     core::Context&,
                                     entt::registry& registry,
                                     int layer_order) {
  if (!layer_json.contains("objects") || !layer_json["objects"].is_array()) {
    return;
  }
  for (const auto& object_json : layer_json["objects"]) {
    const auto entity = registry.create();
    const std::string object_name = object_json.value("name", "Unnamed");
    registry.emplace<engine::ecs::NameComponent>(entity, object_name);
    registry.emplace<engine::ecs::RenderOrderComponent>(entity, layer_order);

    const int gid = object_json.value("gid", 0);
    if (gid == 0) {
      if (object_json.value("point", false) || object_json.value("ellipse", false) ||
          object_json.value("polygon", false)) {
        registry.destroy(entity);
        continue;
      }
      registry.emplace<engine::ecs::TransformComponent>(
          entity,
          glm::vec2(object_json.value("x", 0.0f), object_json.value("y", 0.0f)),
          glm::vec2(1.0f, 1.0f), object_json.value("rotation", 0.0f));
      engine::ecs::ColliderComponent collider;
      collider.size = glm::vec2(object_json.value("width", 0.0f),
                                object_json.value("height", 0.0f));
      collider.is_trigger = object_json.value("trigger", true);
      registry.emplace<engine::ecs::ColliderComponent>(entity, collider);
      registry.emplace<engine::ecs::PhysicsComponent>(entity);
      registry.get<engine::ecs::PhysicsComponent>(entity).use_gravity = false;
      if (const auto tag = GetTileProperty<std::string>(object_json, "tag");
          tag.has_value()) {
        registry.emplace<engine::ecs::TagComponent>(entity, *tag);
      }
      continue;
    }

    const auto tile_info = GetTileInfoByGid(gid);
    if (tile_info.sprite.GetTextureId().empty()) {
      registry.destroy(entity);
      continue;
    }

    glm::vec2 position(object_json.value("x", 0.0f),
                       object_json.value("y", 0.0f));
    const glm::vec2 dst_size(object_json.value("width", 0.0f),
                             object_json.value("height", 0.0f));
    position.y -= dst_size.y;

    const auto source_rect = tile_info.sprite.GetSourceRect();
    if (!source_rect.has_value()) {
      registry.destroy(entity);
      continue;
    }
    const glm::vec2 src_size(source_rect->w, source_rect->h);
    const glm::vec2 scale = dst_size / src_size;
    registry.emplace<engine::ecs::TransformComponent>(
        entity, position, scale, object_json.value("rotation", 0.0f));
    registry.emplace<engine::ecs::SpriteComponent>(
        entity, tile_info.sprite, utils::Alignment::kNone, src_size,
        glm::vec2(0.0f, 0.0f), false);

    const auto tile_json = GetTileJsonByGid(gid);
    if (tile_info.type == engine::ecs::TileType::kSolid) {
      engine::ecs::ColliderComponent collider;
      collider.size = src_size;
      registry.emplace<engine::ecs::ColliderComponent>(entity, collider);
      registry.emplace<engine::ecs::PhysicsComponent>(entity);
      registry.emplace<engine::ecs::TagComponent>(entity, std::string("solid"));
    } else if (tile_json.has_value()) {
      if (const auto rect = GetColliderRect(*tile_json); rect.has_value()) {
        engine::ecs::ColliderComponent collider;
        collider.size = rect->size;
        collider.offset = rect->position;
        registry.emplace<engine::ecs::ColliderComponent>(entity, collider);
        registry.emplace<engine::ecs::PhysicsComponent>(entity);
      }
    }

    if (tile_json.has_value()) {
      if (const auto tag = GetTileProperty<std::string>(*tile_json, "tag");
          tag.has_value()) {
        registry.emplace_or_replace<engine::ecs::TagComponent>(entity, *tag);
      } else if (tile_info.type == engine::ecs::TileType::kHazard) {
        registry.emplace_or_replace<engine::ecs::TagComponent>(entity,
                                                               std::string("hazard"));
      }

      if (const auto gravity = GetTileProperty<bool>(*tile_json, "gravity");
          gravity.has_value()) {
        if (!registry.all_of<engine::ecs::PhysicsComponent>(entity)) {
          registry.emplace<engine::ecs::PhysicsComponent>(entity);
        }
        registry.get<engine::ecs::PhysicsComponent>(entity).use_gravity = *gravity;
      }

      if (const auto health = GetTileProperty<int>(*tile_json, "health");
          health.has_value()) {
        registry.emplace<engine::ecs::HealthComponent>(
            entity, *health, *health, false, 2.0f, 0.0f);
      }

      if (const auto animation_text =
              GetTileProperty<std::string>(*tile_json, "animation");
          animation_text.has_value()) {
        const auto animation_json =
            nlohmann::json::parse(*animation_text, nullptr, false);
        if (!animation_json.is_discarded()) {
          auto& animation_component =
              registry.emplace<engine::ecs::AnimationComponent>(entity);
          AddAnimation(animation_json, animation_component, src_size);
        }
      }
    }
  }
}

void EcsLevelLoader::AddAnimation(
    const nlohmann::json& anim_json,
    engine::ecs::AnimationComponent& animation_component,
    const glm::vec2& sprite_size) const {
  if (!anim_json.is_object()) {
    return;
  }
  for (const auto& animation_item : anim_json.items()) {
    if (!animation_item.value().is_object()) {
      continue;
    }
    auto animation =
      std::make_shared<engine::render::Animation>(animation_item.key());
    const float duration = animation_item.value().value("duration", 100.0f) /
                           1000.0f;
    const int row = animation_item.value().value("row", 0);
    if (!animation_item.value().contains("frames") ||
        !animation_item.value()["frames"].is_array()) {
      continue;
    }
    for (const auto& frame : animation_item.value()["frames"]) {
      if (!frame.is_number_integer()) {
        continue;
      }
      const int column = frame.get<int>();
      animation->AddFrame({static_cast<float>(column) * sprite_size.x,
                           static_cast<float>(row) * sprite_size.y,
                           sprite_size.x, sprite_size.y},
                          duration);
    }
    animation_component.animations.emplace(animation_item.key(), animation);
  }
}

void EcsLevelLoader::LoadTileset(const std::string& tileset_path, int first_gid) {
  std::ifstream tileset_file(tileset_path);
  if (!tileset_file.is_open()) {
    return;
  }
  auto tileset_json = nlohmann::json::parse(tileset_file, nullptr, false);
  if (tileset_json.is_discarded()) {
    return;
  }
  tileset_json["file_path"] = tileset_path;
  tileset_data_[first_gid] = std::move(tileset_json);
}

engine::ecs::TileInfo EcsLevelLoader::GetTileInfoByGid(int gid) const {
  if (gid == 0) {
    return {};
  }
  auto tileset_it = tileset_data_.upper_bound(gid);
  if (tileset_it == tileset_data_.begin()) {
    return {};
  }
  --tileset_it;
  const auto& tileset = tileset_it->second;
  const int local_id = gid - tileset_it->first;
  const std::string file_path = tileset.value("file_path", "");
  if (tileset.contains("image")) {
    const auto texture_id = ResolvePath(tileset.value("image", ""), file_path);
    const int columns = tileset.value("columns", 1);
    const int coord_x = local_id % columns;
    const int coord_y = local_id / columns;
    SDL_FRect texture_rect = {
        static_cast<float>(coord_x * tile_size_.x),
        static_cast<float>(coord_y * tile_size_.y),
        static_cast<float>(tile_size_.x),
        static_cast<float>(tile_size_.y)};
    return {{texture_id, texture_rect}, GetTileTypeById(tileset, local_id)};
  }

  if (!tileset.contains("tiles")) {
    return {};
  }
  for (const auto& tile_json : tileset["tiles"]) {
    if (tile_json.value("id", -1) != local_id ||
        !tile_json.contains("image")) {
      continue;
    }
    const auto texture_id = ResolvePath(tile_json.value("image", ""), file_path);
    SDL_FRect texture_rect = {
        static_cast<float>(tile_json.value("x", 0)),
        static_cast<float>(tile_json.value("y", 0)),
        static_cast<float>(tile_json.value("width", tile_json.value("imagewidth", 0))),
        static_cast<float>(tile_json.value("height", tile_json.value("imageheight", 0)))};
    return {{texture_id, texture_rect}, GetTileType(tile_json)};
  }
  return {};
}

std::string EcsLevelLoader::ResolvePath(const std::string& relative_path,
                                        const std::string& file_path) const {
  if (relative_path.empty()) {
    return {};
  }
  const auto map_dir = std::filesystem::path(file_path).parent_path();
  std::error_code error_code;
  const auto resolved =
      std::filesystem::weakly_canonical(map_dir / relative_path, error_code);
  if (error_code) {
    ENGINE_LOG_ERROR("Failed to resolve path '{}' from '{}': {}", relative_path,
                     file_path, error_code.message());
    return (map_dir / relative_path).string();
  }
  return resolved.string();
}

engine::ecs::TileType EcsLevelLoader::GetTileType(
    const nlohmann::json& tile_json) const {
  if (!tile_json.contains("properties")) {
    return engine::ecs::TileType::kNormal;
  }
  for (const auto& property : tile_json["properties"]) {
    const std::string name = property.value("name", "");
    if (name == "solid") {
      return property.value("value", false) ? engine::ecs::TileType::kSolid
                                              : engine::ecs::TileType::kNormal;
    }
    if (name == "unisolid") {
      return property.value("value", false)
                 ? engine::ecs::TileType::kUnisolid
                 : engine::ecs::TileType::kNormal;
    }
    if (name == "slope") {
      const auto slope_type = property.value("value", "");
      if (slope_type == "0_1") {
        return engine::ecs::TileType::kSlope0_1;
      }
      if (slope_type == "1_0") {
        return engine::ecs::TileType::kSlope1_0;
      }
      if (slope_type == "0_2") {
        return engine::ecs::TileType::kSlope0_2;
      }
      if (slope_type == "2_0") {
        return engine::ecs::TileType::kSlope2_0;
      }
      if (slope_type == "2_1") {
        return engine::ecs::TileType::kSlope2_1;
      }
      if (slope_type == "1_2") {
        return engine::ecs::TileType::kSlope1_2;
      }
    }
    if (name == "hazard") {
      return property.value("value", false) ? engine::ecs::TileType::kHazard
                                              : engine::ecs::TileType::kNormal;
    }
    if (name == "ladder") {
      return property.value("value", false) ? engine::ecs::TileType::kLadder
                                              : engine::ecs::TileType::kNormal;
    }
  }
  return engine::ecs::TileType::kNormal;
}

engine::ecs::TileType EcsLevelLoader::GetTileTypeById(
    const nlohmann::json& tileset_json, int local_id) const {
  if (!tileset_json.contains("tiles")) {
    return engine::ecs::TileType::kNormal;
  }
  for (const auto& tile_json : tileset_json["tiles"]) {
    if (tile_json.value("id", -1) == local_id) {
      return GetTileType(tile_json);
    }
  }
  return engine::ecs::TileType::kNormal;
}

std::optional<utils::Rect> EcsLevelLoader::GetColliderRect(
    const nlohmann::json& tile_json) const {
  if (!tile_json.contains("objectgroup") ||
      !tile_json["objectgroup"].contains("objects")) {
    return std::nullopt;
  }
  for (const auto& object : tile_json["objectgroup"]["objects"]) {
    utils::Rect rect = {
        glm::vec2(object.value("x", 0.0f), object.value("y", 0.0f)),
        glm::vec2(object.value("width", 0.0f), object.value("height", 0.0f))};
    if (rect.size.x > 0.0f && rect.size.y > 0.0f) {
      return rect;
    }
  }
  return std::nullopt;
}

std::optional<nlohmann::json> EcsLevelLoader::GetTileJsonByGid(int gid) const {
  auto tileset_it = tileset_data_.upper_bound(gid);
  if (tileset_it == tileset_data_.begin()) {
    return std::nullopt;
  }
  --tileset_it;
  const auto& tileset = tileset_it->second;
  const int local_id = gid - tileset_it->first;
  if (!tileset.contains("tiles")) {
    return std::nullopt;
  }
  for (const auto& tile_json : tileset["tiles"]) {
    if (tile_json.value("id", -1) == local_id) {
      return tile_json;
    }
  }
  return std::nullopt;
}

}  // namespace engine::scene