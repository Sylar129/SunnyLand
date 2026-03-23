// Copyright Sylar129

#include "engine/resource/tiled_parser.h"

#include <filesystem>
#include <fstream>
#include <optional>
#include <string_view>

#include "utils/log.h"

namespace engine::resource {

namespace {

using json = nlohmann::json;

constexpr std::uint32_t kHorizontalFlipFlag = 0x80000000U;
constexpr std::uint32_t kVerticalFlipFlag = 0x40000000U;
constexpr std::uint32_t kDiagonalFlipFlag = 0x20000000U;
constexpr std::uint32_t kHexagonalRotate120Flag = 0x10000000U;
constexpr std::uint32_t kAllTileTransformFlags =
    kHorizontalFlipFlag | kVerticalFlipFlag | kDiagonalFlipFlag |
    kHexagonalRotate120Flag;

const json* FindMember(const json& value, std::string_view key) {
  const auto it = value.find(std::string(key));
  if (it == value.end()) {
    return nullptr;
  }
  return &(*it);
}

std::string ReadString(const json& value, std::string_view key,
                       const std::string& default_value = "") {
  const json* member = FindMember(value, key);
  if (member != nullptr && member->is_string()) {
    return member->get_ref<const std::string&>();
  }
  return default_value;
}

int ReadInt(const json& value, std::string_view key, int default_value = 0) {
  const json* member = FindMember(value, key);
  if (member == nullptr) {
    return default_value;
  }
  if (member->is_number_integer() || member->is_number_unsigned()) {
    return member->get<int>();
  }
  return default_value;
}

double ReadDouble(const json& value, std::string_view key,
                  double default_value = 0.0) {
  const json* member = FindMember(value, key);
  if (member == nullptr) {
    return default_value;
  }
  if (member->is_number()) {
    return member->get<double>();
  }
  return default_value;
}

bool ReadBool(const json& value, std::string_view key,
              bool default_value = false) {
  const json* member = FindMember(value, key);
  if (member != nullptr && member->is_boolean()) {
    return member->get<bool>();
  }
  return default_value;
}

std::uint32_t ReadUInt32Value(const json& value,
                              std::uint32_t default_value = 0U) {
  if (value.is_number_unsigned() || value.is_number_integer()) {
    return value.get<std::uint32_t>();
  }
  return default_value;
}

std::optional<json> LoadJsonFile(const std::filesystem::path& file_path) {
  std::ifstream file(file_path);
  if (!file.is_open()) {
    ENGINE_LOG_ERROR("Failed to open Tiled JSON file '{}'",
                     file_path.generic_string());
    return std::nullopt;
  }

  json document = json::parse(file, nullptr, false);
  if (document.is_discarded()) {
    ENGINE_LOG_ERROR("Failed to parse Tiled JSON file '{}'",
                     file_path.generic_string());
    return std::nullopt;
  }

  return document;
}

std::filesystem::path ResolvePath(const std::filesystem::path& owner_file,
                                  const std::string& relative_path) {
  if (relative_path.empty()) {
    return {};
  }

  const std::filesystem::path path(relative_path);
  if (path.is_absolute()) {
    return path.lexically_normal();
  }

  return (owner_file.parent_path() / path).lexically_normal();
}

TiledGlobalTileId DecodeGlobalTileId(std::uint32_t raw_gid) {
  TiledGlobalTileId tile_id;
  tile_id.raw = raw_gid;
  tile_id.flipped_horizontally = (raw_gid & kHorizontalFlipFlag) != 0U;
  tile_id.flipped_vertically = (raw_gid & kVerticalFlipFlag) != 0U;
  tile_id.flipped_diagonally = (raw_gid & kDiagonalFlipFlag) != 0U;
  tile_id.rotated_hexagonal_120 = (raw_gid & kHexagonalRotate120Flag) != 0U;
  tile_id.gid = raw_gid & ~kAllTileTransformFlags;
  return tile_id;
}

std::vector<TiledProperty> ParseProperties(const json& value) {
  std::vector<TiledProperty> properties;
  const json* properties_json = FindMember(value, "properties");
  if (properties_json == nullptr || !properties_json->is_array()) {
    return properties;
  }

  properties.reserve(properties_json->size());
  for (const auto& property_json : *properties_json) {
    if (!property_json.is_object()) {
      continue;
    }

    TiledProperty property;
    property.name = ReadString(property_json, "name");
    property.type = ReadString(property_json, "type", "string");

    const json* value_member = FindMember(property_json, "value");
    if (value_member != nullptr) {
      property.value = *value_member;
    }

    properties.push_back(std::move(property));
  }

  return properties;
}

std::vector<TiledPoint> ParsePointArray(const json* points_json) {
  std::vector<TiledPoint> points;
  if (points_json == nullptr || !points_json->is_array()) {
    return points;
  }

  points.reserve(points_json->size());
  for (const auto& point_json : *points_json) {
    if (!point_json.is_object()) {
      continue;
    }

    points.push_back(
        TiledPoint{.x = ReadDouble(point_json, "x"),
                   .y = ReadDouble(point_json, "y")});
  }

  return points;
}

std::vector<TiledGlobalTileId> ParseTileData(
    const json& data_json, const std::string& owner_name,
    const std::string& owner_file) {
  std::vector<TiledGlobalTileId> tile_data;
  if (data_json.is_array()) {
    tile_data.reserve(data_json.size());
    for (const auto& tile_id_json : data_json) {
      tile_data.push_back(DecodeGlobalTileId(ReadUInt32Value(tile_id_json)));
    }
    return tile_data;
  }

  if (data_json.is_string()) {
    ENGINE_LOG_WARN(
        "Layer '{}' in '{}' uses encoded tile data. Current parser supports "
        "JSON array tile data only.",
        owner_name, owner_file);
  }

  return tile_data;
}

TiledLayerType ParseLayerType(const std::string& layer_type) {
  if (layer_type == "tilelayer") {
    return TiledLayerType::kTileLayer;
  }
  if (layer_type == "objectgroup") {
    return TiledLayerType::kObjectGroup;
  }
  if (layer_type == "imagelayer") {
    return TiledLayerType::kImageLayer;
  }
  if (layer_type == "group") {
    return TiledLayerType::kGroup;
  }
  return TiledLayerType::kUnknown;
}

TiledText ParseText(const json& text_json) {
  TiledText text;
  text.text = ReadString(text_json, "text");
  text.font_family = ReadString(text_json, "fontfamily", "sans-serif");
  text.color = ReadString(text_json, "color");
  text.horizontal_alignment = ReadString(text_json, "halign", "left");
  text.vertical_alignment = ReadString(text_json, "valign", "top");
  text.pixel_size = ReadInt(text_json, "pixelsize", 16);
  text.wrap = ReadBool(text_json, "wrap");
  text.bold = ReadBool(text_json, "bold");
  text.italic = ReadBool(text_json, "italic");
  text.underline = ReadBool(text_json, "underline");
  text.strikeout = ReadBool(text_json, "strikeout");
  text.kerning = ReadBool(text_json, "kerning", true);
  return text;
}

TiledObject ParseObject(const json& object_json,
                        const std::filesystem::path& owner_file) {
  TiledObject object;
  object.id = ReadInt(object_json, "id");
  object.name = ReadString(object_json, "name");
  object.type = ReadString(object_json, "type");
  object.class_name = ReadString(object_json, "class");
  object.template_file = ReadString(object_json, "template");
  object.x = ReadDouble(object_json, "x");
  object.y = ReadDouble(object_json, "y");
  object.width = ReadDouble(object_json, "width");
  object.height = ReadDouble(object_json, "height");
  object.rotation = ReadDouble(object_json, "rotation");
  object.visible = ReadBool(object_json, "visible", true);
  object.point = ReadBool(object_json, "point");
  object.ellipse = ReadBool(object_json, "ellipse");
  object.properties = ParseProperties(object_json);
  object.polygon = ParsePointArray(FindMember(object_json, "polygon"));
  object.polyline = ParsePointArray(FindMember(object_json, "polyline"));

  if (!object.template_file.empty()) {
    object.resolved_template_file =
        ResolvePath(owner_file, object.template_file).generic_string();
  }

  const json* gid_member = FindMember(object_json, "gid");
  if (gid_member != nullptr) {
    object.tile = DecodeGlobalTileId(ReadUInt32Value(*gid_member));
  }

  const json* text_member = FindMember(object_json, "text");
  if (text_member != nullptr && text_member->is_object()) {
    object.text = ParseText(*text_member);
  }

  return object;
}

TiledLayer ParseLayer(const json& layer_json,
                      const std::filesystem::path& owner_file) {
  TiledLayer layer;
  layer.id = ReadInt(layer_json, "id");
  layer.name = ReadString(layer_json, "name");
  layer.type = ReadString(layer_json, "type");
  layer.class_name = ReadString(layer_json, "class");
  layer.draw_order = ReadString(layer_json, "draworder", "topdown");
  layer.layer_type = ParseLayerType(layer.type);
  layer.width = ReadInt(layer_json, "width");
  layer.height = ReadInt(layer_json, "height");
  layer.x = ReadDouble(layer_json, "x");
  layer.y = ReadDouble(layer_json, "y");
  layer.offset_x = ReadDouble(layer_json, "offsetx");
  layer.offset_y = ReadDouble(layer_json, "offsety");
  layer.parallax_x = ReadDouble(layer_json, "parallaxx", 1.0);
  layer.parallax_y = ReadDouble(layer_json, "parallaxy", 1.0);
  layer.opacity = ReadDouble(layer_json, "opacity", 1.0);
  layer.visible = ReadBool(layer_json, "visible", true);
  layer.repeat_x = ReadBool(layer_json, "repeatx");
  layer.repeat_y = ReadBool(layer_json, "repeaty");
  layer.tint_color = ReadString(layer_json, "tintcolor");
  layer.image = ReadString(layer_json, "image");
  layer.image_width = ReadInt(layer_json, "imagewidth");
  layer.image_height = ReadInt(layer_json, "imageheight");
  layer.properties = ParseProperties(layer_json);

  if (!layer.image.empty()) {
    layer.resolved_image = ResolvePath(owner_file, layer.image).generic_string();
  }

  const json* data_member = FindMember(layer_json, "data");
  if (data_member != nullptr) {
    layer.tiles = ParseTileData(*data_member, layer.name,
                                owner_file.generic_string());
  }

  const json* chunks_member = FindMember(layer_json, "chunks");
  if (chunks_member != nullptr && chunks_member->is_array()) {
    layer.chunks.reserve(chunks_member->size());
    for (const auto& chunk_json : *chunks_member) {
      if (!chunk_json.is_object()) {
        continue;
      }

      TiledChunk chunk;
      chunk.x = ReadInt(chunk_json, "x");
      chunk.y = ReadInt(chunk_json, "y");
      chunk.width = ReadInt(chunk_json, "width");
      chunk.height = ReadInt(chunk_json, "height");

      const json* chunk_data_member = FindMember(chunk_json, "data");
      if (chunk_data_member != nullptr) {
        chunk.tiles = ParseTileData(*chunk_data_member, layer.name,
                                    owner_file.generic_string());
      }

      layer.chunks.push_back(std::move(chunk));
    }
  }

  const json* objects_member = FindMember(layer_json, "objects");
  if (objects_member != nullptr && objects_member->is_array()) {
    layer.objects.reserve(objects_member->size());
    for (const auto& object_json : *objects_member) {
      if (!object_json.is_object()) {
        continue;
      }
      layer.objects.push_back(ParseObject(object_json, owner_file));
    }
  }

  const json* layers_member = FindMember(layer_json, "layers");
  if (layers_member != nullptr && layers_member->is_array()) {
    layer.layers.reserve(layers_member->size());
    for (const auto& child_layer_json : *layers_member) {
      if (!child_layer_json.is_object()) {
        continue;
      }
      layer.layers.push_back(ParseLayer(child_layer_json, owner_file));
    }
  }

  return layer;
}

TiledTile ParseTile(const json& tile_json, const std::filesystem::path& owner_file) {
  TiledTile tile;
  tile.id = ReadInt(tile_json, "id");
  tile.type = ReadString(tile_json, "type");
  tile.class_name = ReadString(tile_json, "class");
  tile.image = ReadString(tile_json, "image");
  tile.image_width = ReadInt(tile_json, "imagewidth");
  tile.image_height = ReadInt(tile_json, "imageheight");
  tile.width = ReadInt(tile_json, "width");
  tile.height = ReadInt(tile_json, "height");
  tile.x = ReadInt(tile_json, "x");
  tile.y = ReadInt(tile_json, "y");
  tile.probability = ReadDouble(tile_json, "probability", 1.0);
  tile.properties = ParseProperties(tile_json);

  if (!tile.image.empty()) {
    tile.resolved_image = ResolvePath(owner_file, tile.image).generic_string();
  }

  const json* object_group_member = FindMember(tile_json, "objectgroup");
  if (object_group_member != nullptr && object_group_member->is_object()) {
    tile.object_layer = ParseLayer(*object_group_member, owner_file);
  }

  const json* animation_member = FindMember(tile_json, "animation");
  if (animation_member != nullptr && animation_member->is_array()) {
    tile.animation.reserve(animation_member->size());
    for (const auto& frame_json : *animation_member) {
      if (!frame_json.is_object()) {
        continue;
      }
      tile.animation.push_back(TiledTileAnimationFrame{
          .tile_id = ReadInt(frame_json, "tileid"),
          .duration = ReadInt(frame_json, "duration")});
    }
  }

  return tile;
}

TiledTileset ParseTileset(const json& tileset_json,
                         const std::filesystem::path& definition_file) {
  TiledTileset tileset;
  tileset.definition_path = definition_file.generic_string();
  tileset.name = ReadString(tileset_json, "name");
  tileset.type = ReadString(tileset_json, "type", "tileset");
  tileset.class_name = ReadString(tileset_json, "class");
  tileset.image = ReadString(tileset_json, "image");
  tileset.tiled_version = ReadString(tileset_json, "tiledversion");
  tileset.tile_width = ReadInt(tileset_json, "tilewidth");
  tileset.tile_height = ReadInt(tileset_json, "tileheight");
  tileset.spacing = ReadInt(tileset_json, "spacing");
  tileset.margin = ReadInt(tileset_json, "margin");
  tileset.tile_count = ReadInt(tileset_json, "tilecount");
  tileset.columns = ReadInt(tileset_json, "columns");
  tileset.image_width = ReadInt(tileset_json, "imagewidth");
  tileset.image_height = ReadInt(tileset_json, "imageheight");
  tileset.properties = ParseProperties(tileset_json);

  const json* grid_member = FindMember(tileset_json, "grid");
  if (grid_member != nullptr && grid_member->is_object()) {
    tileset.grid.orientation = ReadString(*grid_member, "orientation",
                                          "orthogonal");
    tileset.grid.width = ReadInt(*grid_member, "width");
    tileset.grid.height = ReadInt(*grid_member, "height");
  }

  if (!tileset.image.empty()) {
    tileset.resolved_image =
        ResolvePath(definition_file, tileset.image).generic_string();
  }

  const json* tiles_member = FindMember(tileset_json, "tiles");
  if (tiles_member != nullptr && tiles_member->is_array()) {
    tileset.tiles.reserve(tiles_member->size());
    for (const auto& tile_json : *tiles_member) {
      if (!tile_json.is_object()) {
        continue;
      }
      tileset.tiles.push_back(ParseTile(tile_json, definition_file));
    }
  }

  return tileset;
}

std::optional<TiledTileset> ParseTilesetReference(
    const json& tileset_ref_json, const std::filesystem::path& map_file) {
  const std::string source = ReadString(tileset_ref_json, "source");
  if (!source.empty()) {
    const std::filesystem::path resolved_source = ResolvePath(map_file, source);
    const std::optional<json> tileset_document = LoadJsonFile(resolved_source);
    if (!tileset_document.has_value()) {
      return std::nullopt;
    }

    TiledTileset tileset = ParseTileset(*tileset_document, resolved_source);
    tileset.first_gid = static_cast<std::uint32_t>(ReadInt(tileset_ref_json, "firstgid"));
    tileset.source = source;
    tileset.resolved_source = resolved_source.generic_string();
    return tileset;
  }

  TiledTileset tileset = ParseTileset(tileset_ref_json, map_file);
  tileset.first_gid = static_cast<std::uint32_t>(ReadInt(tileset_ref_json, "firstgid"));
  return tileset;
}

}  // namespace

std::shared_ptr<TiledMap> TiledParser::LoadMap(const std::string& file_path) const {
  const std::filesystem::path map_path(file_path);
  const std::optional<json> document = LoadJsonFile(map_path);
  if (!document.has_value()) {
    return nullptr;
  }

  auto tiled_map = std::make_shared<TiledMap>();
  tiled_map->source_path = map_path.lexically_normal().generic_string();
  tiled_map->type = ReadString(*document, "type", "map");
  tiled_map->class_name = ReadString(*document, "class");
  tiled_map->version = ReadString(*document, "version");
  tiled_map->tiled_version = ReadString(*document, "tiledversion");
  tiled_map->orientation = ReadString(*document, "orientation", "orthogonal");
  tiled_map->render_order = ReadString(*document, "renderorder", "right-down");
  tiled_map->background_color = ReadString(*document, "backgroundcolor");
  tiled_map->stagger_axis = ReadString(*document, "staggeraxis");
  tiled_map->stagger_index = ReadString(*document, "staggerindex");
  tiled_map->width = ReadInt(*document, "width");
  tiled_map->height = ReadInt(*document, "height");
  tiled_map->tile_width = ReadInt(*document, "tilewidth");
  tiled_map->tile_height = ReadInt(*document, "tileheight");
  tiled_map->next_layer_id = ReadInt(*document, "nextlayerid");
  tiled_map->next_object_id = ReadInt(*document, "nextobjectid");
  tiled_map->compression_level = ReadInt(*document, "compressionlevel", -1);
  tiled_map->hex_side_length = ReadInt(*document, "hexsidelength");
  tiled_map->infinite = ReadBool(*document, "infinite");
  tiled_map->properties = ParseProperties(*document);

  const json* layers_member = FindMember(*document, "layers");
  if (layers_member != nullptr && layers_member->is_array()) {
    tiled_map->layers.reserve(layers_member->size());
    for (const auto& layer_json : *layers_member) {
      if (!layer_json.is_object()) {
        continue;
      }
      tiled_map->layers.push_back(ParseLayer(layer_json, map_path));
    }
  }

  const json* tilesets_member = FindMember(*document, "tilesets");
  if (tilesets_member != nullptr && tilesets_member->is_array()) {
    tiled_map->tilesets.reserve(tilesets_member->size());
    for (const auto& tileset_ref_json : *tilesets_member) {
      if (!tileset_ref_json.is_object()) {
        continue;
      }
      const std::optional<TiledTileset> tileset =
          ParseTilesetReference(tileset_ref_json, map_path);
      if (tileset.has_value()) {
        tiled_map->tilesets.push_back(*tileset);
      }
    }
  }

  return tiled_map;
}

}  // namespace engine::resource