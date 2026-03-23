// Copyright Sylar129

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

namespace engine::resource {

struct TiledPoint final {
  double x = 0.0;
  double y = 0.0;
};

struct TiledProperty final {
  std::string name;
  std::string type = "string";
  nlohmann::json value;

  bool AsBool(bool default_value = false) const {
    if (value.is_boolean()) {
      return value.get<bool>();
    }
    return default_value;
  }

  std::int64_t AsInt(std::int64_t default_value = 0) const {
    if (value.is_number_integer() || value.is_number_unsigned()) {
      return value.get<std::int64_t>();
    }
    return default_value;
  }

  double AsDouble(double default_value = 0.0) const {
    if (value.is_number()) {
      return value.get<double>();
    }
    return default_value;
  }

  std::string AsString(const std::string& default_value = "") const {
    if (value.is_string()) {
      return value.get<std::string>();
    }
    if (!value.is_null()) {
      return value.dump();
    }
    return default_value;
  }
};

struct TiledGlobalTileId final {
  std::uint32_t raw = 0;
  std::uint32_t gid = 0;
  bool flipped_horizontally = false;
  bool flipped_vertically = false;
  bool flipped_diagonally = false;
  bool rotated_hexagonal_120 = false;

  bool IsEmpty() const { return gid == 0; }
};

struct TiledText final {
  std::string text;
  std::string font_family = "sans-serif";
  std::string color;
  std::string horizontal_alignment = "left";
  std::string vertical_alignment = "top";
  int pixel_size = 16;
  bool wrap = false;
  bool bold = false;
  bool italic = false;
  bool underline = false;
  bool strikeout = false;
  bool kerning = true;
};

struct TiledObject final {
  int id = 0;
  std::string name;
  std::string type;
  std::string class_name;
  std::string template_file;
  std::string resolved_template_file;
  double x = 0.0;
  double y = 0.0;
  double width = 0.0;
  double height = 0.0;
  double rotation = 0.0;
  bool visible = true;
  bool point = false;
  bool ellipse = false;
  std::optional<TiledGlobalTileId> tile;
  std::optional<TiledText> text;
  std::vector<TiledPoint> polygon;
  std::vector<TiledPoint> polyline;
  std::vector<TiledProperty> properties;
};

struct TiledChunk final {
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  std::vector<TiledGlobalTileId> tiles;
};

enum class TiledLayerType {
  kUnknown,
  kTileLayer,
  kObjectGroup,
  kImageLayer,
  kGroup,
};

struct TiledLayer final {
  int id = 0;
  std::string name;
  std::string type;
  std::string class_name;
  std::string draw_order = "topdown";
  TiledLayerType layer_type = TiledLayerType::kUnknown;
  int width = 0;
  int height = 0;
  double x = 0.0;
  double y = 0.0;
  double offset_x = 0.0;
  double offset_y = 0.0;
  double parallax_x = 1.0;
  double parallax_y = 1.0;
  double opacity = 1.0;
  bool visible = true;
  bool repeat_x = false;
  bool repeat_y = false;
  std::string tint_color;
  std::string image;
  std::string resolved_image;
  int image_width = 0;
  int image_height = 0;
  std::vector<TiledProperty> properties;
  std::vector<TiledGlobalTileId> tiles;
  std::vector<TiledChunk> chunks;
  std::vector<TiledObject> objects;
  std::vector<TiledLayer> layers;

  bool IsTileLayer() const { return layer_type == TiledLayerType::kTileLayer; }
  bool IsObjectLayer() const {
    return layer_type == TiledLayerType::kObjectGroup;
  }
  bool IsImageLayer() const {
    return layer_type == TiledLayerType::kImageLayer;
  }
  bool IsGroupLayer() const { return layer_type == TiledLayerType::kGroup; }
};

struct TiledGrid final {
  std::string orientation = "orthogonal";
  int width = 0;
  int height = 0;
};

struct TiledTileAnimationFrame final {
  int tile_id = 0;
  int duration = 0;
};

struct TiledTile final {
  int id = 0;
  std::string type;
  std::string class_name;
  std::string image;
  std::string resolved_image;
  int image_width = 0;
  int image_height = 0;
  int width = 0;
  int height = 0;
  int x = 0;
  int y = 0;
  double probability = 1.0;
  std::vector<TiledProperty> properties;
  std::optional<TiledLayer> object_layer;
  std::vector<TiledTileAnimationFrame> animation;
};

struct TiledTileset final {
  std::uint32_t first_gid = 0;
  std::string source;
  std::string resolved_source;
  std::string definition_path;
  std::string name;
  std::string type;
  std::string class_name;
  std::string image;
  std::string resolved_image;
  std::string tiled_version;
  int tile_width = 0;
  int tile_height = 0;
  int spacing = 0;
  int margin = 0;
  int tile_count = 0;
  int columns = 0;
  int image_width = 0;
  int image_height = 0;
  TiledGrid grid;
  std::vector<TiledProperty> properties;
  std::vector<TiledTile> tiles;
};

struct TiledMap final {
  std::string source_path;
  std::string type;
  std::string class_name;
  std::string version;
  std::string tiled_version;
  std::string orientation = "orthogonal";
  std::string render_order = "right-down";
  std::string background_color;
  std::string stagger_axis;
  std::string stagger_index;
  int width = 0;
  int height = 0;
  int tile_width = 0;
  int tile_height = 0;
  int next_layer_id = 0;
  int next_object_id = 0;
  int compression_level = -1;
  int hex_side_length = 0;
  bool infinite = false;
  std::vector<TiledProperty> properties;
  std::vector<TiledLayer> layers;
  std::vector<TiledTileset> tilesets;

  const TiledTileset* FindTilesetByGid(std::uint32_t gid) const {
    if (gid == 0) {
      return nullptr;
    }

    const TiledTileset* matched_tileset = nullptr;
    for (const auto& tileset : tilesets) {
      if (tileset.first_gid <= gid &&
          (matched_tileset == nullptr ||
           matched_tileset->first_gid < tileset.first_gid)) {
        matched_tileset = &tileset;
      }
    }
    return matched_tileset;
  }

  const TiledTile* FindTileByGid(std::uint32_t gid) const {
    const TiledTileset* tileset = FindTilesetByGid(gid);
    if (tileset == nullptr) {
      return nullptr;
    }

    const int local_tile_id = static_cast<int>(gid - tileset->first_gid);
    for (const auto& tile : tileset->tiles) {
      if (tile.id == local_tile_id) {
        return &tile;
      }
    }
    return nullptr;
  }
};

}  // namespace engine::resource