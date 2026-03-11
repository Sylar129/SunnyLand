// Copyright Sylar129

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "nlohmann/json.hpp"

namespace engine::core {

struct Config final {
  struct Window {
    std::string title = "SunnyLand";
    int width = 1280;
    int height = 720;
    bool resizable = true;
  };

  struct Graphics {
    bool vsync = true;
  };

  struct Performance {
    int target_fps = 144;  ///< @brief Target FPS setting, 0 means unlimited
  };

  struct Audio {
    float music_volume = 0.5f;
    float sound_volume = 0.5f;
  };

  Window window;
  Graphics graphics;
  Performance performance;
  Audio audio;

  using InputMapping =
      std::unordered_map<std::string, std::vector<std::string>>;
  InputMapping input_mappings = {
      {"move_left", {"A", "Left"}}, {"move_right", {"D", "Right"}},
      {"move_up", {"W", "Up"}},     {"move_down", {"S", "Down"}},
      {"jump", {"J", "Space"}},     {"attack", {"K", "MouseLeft"}},
      {"pause", {"P", "Escape"}},
      // More default actions can be added here
  };
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Config::Window, title, width,
                                                height, resizable);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Config::Graphics, vsync);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Config::Performance,
                                                target_fps);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Config::Audio, music_volume,
                                                sound_volume);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Config, window, graphics,
                                                performance, audio,
                                                input_mappings);

}  // namespace engine::core
