// Copyright Sylar129

#include "engine/scene/level_loader.h"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "engine/component/animation_component.h"
#include "engine/component/collider_component.h"
#include "engine/component/health_component.h"
#include "engine/component/parallax_component.h"
#include "engine/component/physics_component.h"
#include "engine/component/sprite_component.h"
#include "engine/component/tilelayer_component.h"
#include "engine/component/transform_component.h"
#include "engine/core/context.h"
#include "engine/object/game_object.h"
#include "engine/physics/collider.h"
#include "engine/render/animation.h"
#include "engine/scene/scene.h"
#include "nlohmann/json.hpp"
#include "utils/log.h"

namespace engine::scene {

bool LevelLoader::LoadLevel(const std::string& level_path, Scene& scene) {
  map_path_ = level_path;

  std::ifstream file(level_path);
  if (!file.is_open()) {
    ENGINE_LOG_ERROR("Can't open level file: {}", level_path);
    return false;
  }

  nlohmann::json json_data;
  try {
    file >> json_data;
  } catch (const nlohmann::json::parse_error& e) {
    ENGINE_LOG_ERROR("Parsing JSON failed: {}", e.what());
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
    ENGINE_LOG_ERROR("Missing or invalid 'layers' in map '{}'", level_path);
    return false;
  }
  for (const auto& layer_json : json_data["layers"]) {
    std::string layer_type = layer_json.value("type", "none");
    if (!layer_json.value("visible", true)) {
      ENGINE_LOG_INFO("Layer '{}' is invisible. Skipping.",
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
      ENGINE_LOG_WARN("Unsupported layer type: {}", layer_type);
    }
  }

  ENGINE_LOG_INFO("Level [{}] loaded.", level_path);
  return true;
}

void LevelLoader::LoadImageLayer(const nlohmann::json& layer_json,
                                 Scene& scene) {
  const std::string& image_path = layer_json.value("image", "");
  if (image_path.empty()) {
    ENGINE_LOG_ERROR("Missing 'image' in Layer '{}'",
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

  auto game_object = std::make_unique<object::GameObject>(layer_name);

  game_object->AddComponent<component::TransformComponent>(offset);
  game_object->AddComponent<component::ParallaxComponent>(
      texture_id, scroll_factor, repeat);

  scene.AddGameObject(std::move(game_object));
  ENGINE_LOG_INFO("Loading Layer '{}' completed.", layer_name);
}

void LevelLoader::LoadTileLayer(const nlohmann::json& layer_json,
                                Scene& scene) {
  std::vector<component::TileInfo> tiles;
  tiles.reserve(static_cast<size_t>(map_size_.x) *
                static_cast<size_t>(map_size_.y));

  const auto& data = layer_json["data"];

  for (const auto& gid : data) {
    tiles.push_back(GetTileInfoByGid(gid));
  }

  auto game_object =
      std::make_unique<object::GameObject>(layer_json.value("name", "Unnamed"));
  game_object->AddComponent<component::TileLayerComponent>(
      tile_size_, map_size_, std::move(tiles));
  scene.AddGameObject(std::move(game_object));
}

void LevelLoader::LoadObjectLayer(const nlohmann::json& layer_json,
                                  Scene& scene) {
  if (!layer_json.contains("objects") || !layer_json["objects"].is_array()) {
    ENGINE_LOG_ERROR("Object layer '{}' missing 'objects' attribute.",
                     layer_json.value("name", "Unnamed"));
    return;
  }

  const auto& objects = layer_json["objects"];
  for (const auto& object : objects) {
    auto gid = object.value("gid", 0);

    if (gid == 0) {
      if (object.value("point", false)) {
        continue;
      } else if (object.value("ellipse", false)) {
        continue;
      } else if (object.value("polygon", false)) {
        continue;
      } else {
        const std::string& object_name = object.value("name", "Unnamed");
        auto game_object = std::make_unique<object::GameObject>(object_name);
        auto position =
            glm::vec2(object.value("x", 0.0f), object.value("y", 0.0f));
        auto dst_size = glm::vec2(object.value("width", 0.0f),
                                  object.value("height", 0.0f));
        auto rotation = object.value("rotation", 0.0f);
        game_object->AddComponent<component::TransformComponent>(
            position, glm::vec2(1.0f), rotation);

        auto collider = std::make_unique<physics::AABBCollider>(dst_size);
        auto* cc = game_object->AddComponent<component::ColliderComponent>(
            std::move(collider));
        cc->SetTrigger(object.value("trigger", true));
        game_object->AddComponent<component::PhysicsComponent>(
            &scene.GetContext().GetPhysicsEngine(), false);

        if (auto tag = GetTileProperty<std::string>(object, "tag"); tag) {
          game_object->SetTag(tag.value());
        }
        scene.AddGameObject(std::move(game_object));
        ENGINE_LOG_INFO("Load object: '{}' completed.", object_name);
      }
    } else {
      auto tile_info = GetTileInfoByGid(gid);
      if (tile_info.sprite.GetTextureId().empty()) {
        ENGINE_LOG_ERROR("Tile {} does not have texture.", gid);
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
        ENGINE_LOG_ERROR("Tile {} does not have source rect.", gid);
        continue;
      }
      auto src_size = glm::vec2(src_size_opt->w, src_size_opt->h);
      auto scale = dst_size / src_size;

      const std::string& object_name = object.value("name", "Unnamed");

      auto game_object = std::make_unique<object::GameObject>(object_name);
      game_object->AddComponent<component::TransformComponent>(position, scale,
                                                               rotation);
      game_object->AddComponent<component::SpriteComponent>(
          std::move(tile_info.sprite), scene.GetContext().GetResourceManager());

      auto tile_json = GetTileJsonByGid(gid);

      if (tile_info.physics.collision == component::TileCollisionType::kSolid) {
        auto collider = std::make_unique<physics::AABBCollider>(src_size);
        game_object->AddComponent<component::ColliderComponent>(
            std::move(collider));
        game_object->AddComponent<component::PhysicsComponent>(
            &scene.GetContext().GetPhysicsEngine(), false);
        game_object->SetTag("solid");
      } else if (auto rect = GetColliderRect(tile_json); rect) {
        auto collider = std::make_unique<physics::AABBCollider>(rect->size);
        auto* cc = game_object->AddComponent<component::ColliderComponent>(
            std::move(collider));
        cc->SetOffset(rect->position);
        game_object->AddComponent<component::PhysicsComponent>(
            &scene.GetContext().GetPhysicsEngine(), false);
      }

      auto tag = GetTileProperty<std::string>(tile_json, "tag");
      if (tag) {
        game_object->SetTag(tag.value());
      } else if (!tile_info.trigger_tag.empty()) {
        game_object->SetTag(tile_info.trigger_tag);
      }

      auto gravity = GetTileProperty<bool>(tile_json, "gravity");
      if (gravity) {
        auto pc = game_object->GetComponent<component::PhysicsComponent>();
        if (pc) {
          pc->SetUseGravity(gravity.value());
        } else {
          ENGINE_LOG_WARN(
              "Object '{}' does not have PhysicsComponent when setting gravity "
              "property.",
              object_name);
          game_object->AddComponent<component::PhysicsComponent>(
              &scene.GetContext().GetPhysicsEngine(), gravity.value());
        }
      }

      auto anim_string = GetTileProperty<std::string>(tile_json, "animation");
      if (anim_string) {
        nlohmann::json anim_json;
        try {
          anim_json = nlohmann::json::parse(anim_string.value());
        } catch (const nlohmann::json::parse_error& e) {
          ENGINE_LOG_ERROR("Parsing animation JSON failed for object '{}': {}",
                           object_name, e.what());
          continue;
        }
        auto* ac = game_object->AddComponent<component::AnimationComponent>();
        AddAnimation(anim_json, ac, src_size);
      }

      auto health = GetTileProperty<int>(tile_json, "health");
      if (health) {
        game_object->AddComponent<component::HealthComponent>(health.value());
      }

      scene.AddGameObject(std::move(game_object));
      ENGINE_LOG_INFO("Load object: '{}' completed.", object_name);
    }
  }
}

void LevelLoader::AddAnimation(const nlohmann::json& anim_json,
                               component::AnimationComponent* ac,
                               const glm::vec2& sprite_size) {
  if (!anim_json.is_object() || !ac) {
    ENGINE_LOG_ERROR("Invalid animation JSON or AnimationComponent is null.");
    return;
  }
  for (const auto& anim : anim_json.items()) {
    const std::string& anim_name = anim.key();
    const auto& anim_info = anim.value();
    if (!anim_info.is_object()) {
      ENGINE_LOG_WARN("Animation '{}' info is not a JSON object. Skipping.",
                      anim_name);
      continue;
    }
    auto duration_ms = anim_info.value("duration", 100.0f);
    auto duration = duration_ms / 1000.0f;
    auto row = anim_info.value("row", 0);
    if (!anim_info.contains("frames") || !anim_info["frames"].is_array()) {
      ENGINE_LOG_WARN("Animation '{}' missing 'frames' array. Skipping.",
                      anim_name);
      continue;
    }
    auto animation = std::make_unique<render::Animation>(anim_name);

    for (const auto& frame : anim_info["frames"]) {
      if (!frame.is_number_integer()) {
        ENGINE_LOG_WARN(
            "Animation '{}' has a non-integer frame index. Skipping this "
            "frame.",
            anim_name);
        continue;
        ;
      }
      auto column = frame.get<int>();
      SDL_FRect src_rect = {column * sprite_size.x, row * sprite_size.y,
                            sprite_size.x, sprite_size.y};
      animation->AddFrame(src_rect, duration);
    }
    ac->AddAnimation(std::move(animation));
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

component::TileInfo LevelLoader::GetTileInfoByGid(int gid) const {
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
    const nlohmann::json* tile_json = nullptr;
    if (tileset.contains("tiles")) {
      for (const auto& tile : tileset["tiles"]) {
        if (tile.value("id", -1) == local_id) {
          tile_json = &tile;
          break;
        }
      }
    }
    return ResolveTileInfo({texture_id, texture_rect}, tile_json);
  } else {
    if (!tileset.contains("tiles")) {
      ENGINE_LOG_ERROR("Tileset '{}' missing 'tiles' attribute.",
                       tileset_it->first);
      return component::TileInfo();
    }

    const auto& tiles_json = tileset["tiles"];
    for (const auto& tile_json : tiles_json) {
      auto tile_id = tile_json.value("id", 0);
      if (tile_id == local_id) {
        if (!tile_json.contains("image")) {
          ENGINE_LOG_ERROR("Tileset '{}' missing 'image' attribute.",
                           tileset_it->first, tile_id);
          return component::TileInfo();
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
        render::Sprite sprite{texture_id, texture_rect};
        return ResolveTileInfo(std::move(sprite), &tile_json);
      }
    }
  }
  return {};
}

component::TileInfo LevelLoader::ResolveTileInfo(
    render::Sprite sprite, const nlohmann::json* tile_json) const {
  component::TileInfo tile_info(std::move(sprite));
  if (!tile_json) {
    return tile_info;
  }

  tile_info.physics = GetTilePhysics(*tile_json);
  if (auto trigger_tag =
          GetTileProperty<std::string>(*tile_json, "trigger_tag");
      trigger_tag) {
    tile_info.trigger_tag = trigger_tag.value();
  }

  if (tile_info_resolver_) {
    tile_info_resolver_(*tile_json, tile_info);
  }

  return tile_info;
}

std::string LevelLoader::ResolvePath(const std::string& relative_path,
                                     const std::string& file_path) const {
  auto map_dir = std::filesystem::path(file_path).parent_path();
  auto final_path = std::filesystem::canonical(map_dir / relative_path);
  return final_path.string();
}

component::TilePhysics LevelLoader::GetTilePhysics(
    const nlohmann::json& tile_json) const {
  component::TilePhysics tile_physics;

  if (auto collision = GetTileProperty<std::string>(tile_json, "collision");
      collision) {
    if (collision.value() == "solid") {
      tile_physics.collision = component::TileCollisionType::kSolid;
    } else if (collision.value() == "one_way" ||
               collision.value() == "oneway") {
      tile_physics.collision = component::TileCollisionType::kOneWay;
    } else if (collision.value() != "none") {
      ENGINE_LOG_WARN("Unknown tile collision type: {}", collision.value());
    }
  }

  if (auto slope_heights =
          GetTileProperty<std::string>(tile_json, "slope_heights");
      slope_heights) {
    if (!ParseSlopeHeights(slope_heights.value(), tile_physics)) {
      ENGINE_LOG_WARN("Invalid slope_heights format: {}",
                      slope_heights.value());
    }
  }

  if (auto climbable = GetTileProperty<bool>(tile_json, "climbable");
      climbable && climbable.value()) {
    tile_physics.is_climbable = true;
  }

  return tile_physics;
}

bool LevelLoader::ParseSlopeHeights(
    const std::string& slope_heights,
    component::TilePhysics& tile_physics) const {
  std::stringstream stream(slope_heights);
  std::string left_height;
  std::string right_height;

  if (!std::getline(stream, left_height, ',') ||
      !std::getline(stream, right_height, ',')) {
    return false;
  }

  try {
    tile_physics.collision = component::TileCollisionType::kSlope;
    tile_physics.slope.left_height = std::stof(left_height);
    tile_physics.slope.right_height = std::stof(right_height);
  } catch (const std::exception&) {
    return false;
  }

  return tile_physics.slope.left_height >= 0.0f &&
         tile_physics.slope.left_height <= 1.0f &&
         tile_physics.slope.right_height >= 0.0f &&
         tile_physics.slope.right_height <= 1.0f;
}

std::optional<utils::Rect> LevelLoader::GetColliderRect(
    const nlohmann::json& tile_json) const {
  if (!tile_json.contains("objectgroup")) return std::nullopt;
  auto& objectgroup = tile_json["objectgroup"];
  if (!objectgroup.contains("objects")) return std::nullopt;
  auto& objects = objectgroup["objects"];
  for (const auto& object : objects) {
    auto rect = utils::Rect(
        glm::vec2(object.value("x", 0.0f), object.value("y", 0.0f)),
        glm::vec2(object.value("width", 0.0f), object.value("height", 0.0f)));
    if (rect.size.x > 0 && rect.size.y > 0) {
      return rect;
    }
  }
  return std::nullopt;
}

std::optional<nlohmann::json> LevelLoader::GetTileJsonByGid(int gid) const {
  auto tileset_it = tileset_data_.upper_bound(gid);
  if (tileset_it == tileset_data_.begin()) {
    ENGINE_LOG_ERROR("Tileset '{}' not found.", gid);
    return std::nullopt;
  }
  --tileset_it;
  const auto& tileset = tileset_it->second;
  auto local_id = gid - tileset_it->first;
  if (!tileset.contains("tiles")) {
    ENGINE_LOG_ERROR("Tileset '{}' missing 'tiles' attribute.",
                     tileset_it->first);
    return std::nullopt;
  }
  const auto& tiles_json = tileset["tiles"];
  for (const auto& tile_json : tiles_json) {
    auto tile_id = tile_json.value("id", 0);
    if (tile_id == local_id) {
      return tile_json;
    }
  }
  return std::nullopt;
}

}  // namespace engine::scene
