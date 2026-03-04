// Copyright Sylar129

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "nlohmann/json_fwd.hpp"

namespace engine::core {

/**
 * @brief Manages application configuration settings.
 *
 * Provides default values for configuration items and supports
 * loading/saving configuration from/to JSON files.
 * Default values are used if loading fails or the file does not exist.
 */
class Config final {
 public:
  // --- Default configuration values --- (All public for easy extension)
  // Window settings
  std::string window_title_ = "SunnyLand";
  int window_width_ = 1280;
  int window_height_ = 720;
  bool window_resizable_ = true;

  // Graphics settings
  bool vsync_enabled_ = true;  ///< @brief Whether to enable vertical sync

  // Performance settings
  int target_fps_ = 144;  ///< @brief Target FPS setting, 0 means unlimited

  // Audio settings
  float music_volume_ = 0.5f;
  float sound_volume_ = 0.5f;

  // Maps action names to lists of SDL Scancode names
  std::unordered_map<std::string, std::vector<std::string>> input_mappings_ = {
      // Provides reasonable default values in case the config file fails to
      // load or this section is missing
      {"move_left", {"A", "Left"}}, {"move_right", {"D", "Right"}},
      {"move_up", {"W", "Up"}},     {"move_down", {"S", "Down"}},
      {"jump", {"J", "Space"}},     {"attack", {"K", "MouseLeft"}},
      {"pause", {"P", "Escape"}},
      // More default actions can be added here
  };

  ///< @brief Constructor specifying the config file path.
  explicit Config(const std::string& filepath);

  // Delete copy and move semantics
  Config(const Config&) = delete;
  Config& operator=(const Config&) = delete;
  Config(Config&&) = delete;
  Config& operator=(Config&&) = delete;

  ///< @brief Loads configuration from the specified JSON
  ///< file. Returns true on success, false otherwise.
  bool LoadFromFile(const std::string& filepath);

  ///< @brief Saves current configuration to the specified
  ///< JSON file. Returns true on success, false otherwise.
  [[nodiscard]] bool SaveToFile(const std::string& filepath);

 private:
  ///< @brief Deserializes configuration from a JSON object.
  void FromJson(const nlohmann::json& j);

  ///< @brief Converts current configuration to a JSON object
  ///< (ordered).
  nlohmann::ordered_json ToJson() const;
};

}  // namespace engine::core