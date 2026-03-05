// Copyright Sylar129

#include "engine/core/config.h"

#include <fstream>

#include "log.h"
#include "nlohmann/json.hpp"

namespace engine::core {

Config::Config(const std::string& filepath) { LoadFromFile(filepath); }

bool Config::LoadFromFile(const std::string& filepath) {
  std::ifstream file(filepath);
  if (!file.is_open()) {
    ENGINE_WARN(
        "Config file '{}' not found. Using default settings and creating "
        "default config file.",
        filepath);
    if (!SaveToFile(filepath)) {
      ENGINE_ERROR("Unable to create default config file '{}'.", filepath);
      return false;
    }
    return false;  // File does not exist, use default values
  }

  try {
    nlohmann::json j;
    file >> j;
    FromJson(j);
    ENGINE_INFO("Successfully loaded config from '{}'.", filepath);
    return true;
  } catch (const std::exception& e) {
    ENGINE_ERROR("Error reading config file '{}': {}. Using default settings.",
                 filepath, e.what());
  }
  return false;
}

bool Config::SaveToFile(const std::string& filepath) {
  std::ofstream file(filepath);
  if (!file.is_open()) {
    ENGINE_ERROR("Unable to open config file '{}' for writing.", filepath);
    return false;
  }

  try {
    nlohmann::ordered_json j = ToJson();
    file << j.dump(4);
    ENGINE_INFO("Successfully saved config to '{}'.", filepath);
    return true;
  } catch (const std::exception& e) {
    ENGINE_ERROR("Error writing config file '{}': {}", filepath, e.what());
  }
  return false;
}

void Config::FromJson(const nlohmann::json& j) {
  if (j.contains("window")) {
    const auto& window_config = j["window"];
    window_title_ = window_config.value("title", window_title_);
    window_width_ = window_config.value("width", window_width_);
    window_height_ = window_config.value("height", window_height_);
    window_resizable_ = window_config.value("resizable", window_resizable_);
  }
  if (j.contains("graphics")) {
    const auto& graphics_config = j["graphics"];
    vsync_enabled_ = graphics_config.value("vsync", vsync_enabled_);
  }
  if (j.contains("performance")) {
    const auto& perf_config = j["performance"];
    target_fps_ = perf_config.value("target_fps", target_fps_);
    if (target_fps_ < 0) {
      ENGINE_WARN("Target FPS cannot be negative. Setting to 0 (unlimited).");
      target_fps_ = 0;
    }
  }
  if (j.contains("audio")) {
    const auto& audio_config = j["audio"];
    music_volume_ = audio_config.value("music_volume", music_volume_);
    sound_volume_ = audio_config.value("sound_volume", sound_volume_);
  }

  // Load input_mappings from JSON
  if (j.contains("input_mappings") && j["input_mappings"].is_object()) {
    const auto& mappings_json = j["input_mappings"];
    try {
      // Directly attempt to convert from JSON object to map<string,
      // vector<string>>
      auto input_mappings =
          mappings_json
              .get<std::unordered_map<std::string, std::vector<std::string>>>();
      // If successful, move input_mappings to input_mappings_
      input_mappings_ = std::move(input_mappings);
      ENGINE_TRACE("Successfully loaded input mappings from config.");
    } catch (const std::exception& e) {
      ENGINE_WARN(
          "Config load warning: Exception occurred while parsing "
          "'input_mappings'. "
          "Using default mappings. Error: {}",
          e.what());
    }
  } else {
    ENGINE_TRACE(
        "Config trace: 'input_mappings' section not found or not an object. "
        "Using default mappings defined in header file.");
  }
}

nlohmann::ordered_json Config::ToJson() const {
  return nlohmann::ordered_json{
      {"window",
       {{"title", window_title_},
        {"width", window_width_},
        {"height", window_height_},
        {"resizable", window_resizable_}}},
      {"graphics", {{"vsync", vsync_enabled_}}},
      {"performance", {{"target_fps", target_fps_}}},
      {"audio",
       {{"music_volume", music_volume_}, {"sound_volume", sound_volume_}}},
      {"input_mappings", input_mappings_}};
}

}  // namespace engine::core
