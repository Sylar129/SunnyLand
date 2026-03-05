// Copyright Sylar129
// Config module unit tests

#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "engine/core/config.h"

namespace fs = std::filesystem;

class ConfigTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Create a temporary test config file path
    test_config_file_ = "test_config.json";
    // Clean up any existing test file
    if (fs::exists(test_config_file_)) {
      fs::remove(test_config_file_);
    }
  }

  void TearDown() override {
    // Clean up test file after each test
    if (fs::exists(test_config_file_)) {
      fs::remove(test_config_file_);
    }
  }

  std::string test_config_file_;
};

// Test 1: Config initialization with default values
TEST_F(ConfigTest, DefaultValuesInitialization) {
  engine::core::Config config(test_config_file_);
  
  EXPECT_EQ(config.window_title_, "SunnyLand");
  EXPECT_EQ(config.window_width_, 1280);
  EXPECT_EQ(config.window_height_, 720);
  EXPECT_TRUE(config.window_resizable_);
  EXPECT_TRUE(config.vsync_enabled_);
  EXPECT_EQ(config.target_fps_, 144);
  EXPECT_FLOAT_EQ(config.music_volume_, 0.5f);
  EXPECT_FLOAT_EQ(config.sound_volume_, 0.5f);
}

// Test 2: Config file creation and saving
TEST_F(ConfigTest, SaveToFile) {
  engine::core::Config config(test_config_file_);
  
  // Modify some values
  config.window_title_ = "Test Window";
  config.window_width_ = 1920;
  config.target_fps_ = 60;
  
  // Save to file
  bool result = config.SaveToFile(test_config_file_);
  EXPECT_TRUE(result);
  EXPECT_TRUE(fs::exists(test_config_file_));
}

// Test 3: Config loading from file
TEST_F(ConfigTest, LoadFromFile) {
  // First create and save a config
  {
    engine::core::Config config(test_config_file_);
    config.window_title_ = "Loaded Config";
    config.window_width_ = 1600;
    config.target_fps_ = 120;
    config.SaveToFile(test_config_file_);
  }
  
  // Then load it in a new instance
  engine::core::Config loaded_config(test_config_file_);
  
  EXPECT_EQ(loaded_config.window_title_, "Loaded Config");
  EXPECT_EQ(loaded_config.window_width_, 1600);
  EXPECT_EQ(loaded_config.target_fps_, 120);
}

// Test 4: Invalid FPS handling
TEST_F(ConfigTest, InvalidFpsHandling) {
  // Create a config file with negative FPS manually
  std::ofstream file(test_config_file_);
  file << R"({
    "window": {
      "title": "Test",
      "width": 1280,
      "height": 720,
      "resizable": true
    },
    "graphics": {"vsync": true},
    "performance": {"target_fps": -1},
    "audio": {"music_volume": 0.5, "sound_volume": 0.5},
    "input_mappings": {}
  })";
  file.close();
  
  engine::core::Config config(test_config_file_);
  bool result = config.LoadFromFile(test_config_file_);
  
  // Should load successfully but FPS should be corrected to 0
  EXPECT_EQ(config.target_fps_, 0);
}

// Test 5: Input mappings preservation
TEST_F(ConfigTest, InputMappingsPreservation) {
  {
    engine::core::Config config(test_config_file_);
    
    // Verify default input mappings exist
    EXPECT_TRUE(config.input_mappings_.contains("move_left"));
    EXPECT_TRUE(config.input_mappings_.contains("jump"));
    
    // Save to file
    config.SaveToFile(test_config_file_);
  }
  
  // Load and verify
  engine::core::Config loaded_config(test_config_file_);
  EXPECT_TRUE(loaded_config.input_mappings_.contains("move_left"));
  EXPECT_TRUE(loaded_config.input_mappings_.contains("jump"));
}

// Test 6: Volume range boundaries
TEST_F(ConfigTest, VolumeValues) {
  engine::core::Config config(test_config_file_);
  
  // Set and verify volume values
  config.music_volume_ = 1.0f;
  config.sound_volume_ = 0.0f;
  
  EXPECT_FLOAT_EQ(config.music_volume_, 1.0f);
  EXPECT_FLOAT_EQ(config.sound_volume_, 0.0f);
  
  bool result = config.SaveToFile(test_config_file_);
  EXPECT_TRUE(result);
}

// Test 7: Non-existent file handling
TEST_F(ConfigTest, NonExistentFileCreation) {
  std::string non_existent = "non_existent_dir/config.json";
  
  engine::core::Config config(non_existent);
  
  // Should use default values but may not save if directory doesn't exist
  EXPECT_EQ(config.window_title_, "SunnyLand");
  EXPECT_EQ(config.target_fps_, 144);
}

// Test 8: Window configuration
TEST_F(ConfigTest, WindowConfiguration) {
  engine::core::Config config(test_config_file_);
  
  config.window_width_ = 2560;
  config.window_height_ = 1440;
  config.window_resizable_ = false;
  
  EXPECT_EQ(config.window_width_, 2560);
  EXPECT_EQ(config.window_height_, 1440);
  EXPECT_FALSE(config.window_resizable_);
}

// Test 9: Graphics settings
TEST_F(ConfigTest, GraphicsSettings) {
  engine::core::Config config(test_config_file_);
  
  config.vsync_enabled_ = false;
  EXPECT_FALSE(config.vsync_enabled_);
  
  config.vsync_enabled_ = true;
  EXPECT_TRUE(config.vsync_enabled_);
}

// Test 10: Multiple save/load cycles
TEST_F(ConfigTest, MultipleSaveLoadCycles) {
  for (int i = 0; i < 3; ++i) {
    engine::core::Config config(test_config_file_);
    config.target_fps_ = 60 + (i * 20);
    config.SaveToFile(test_config_file_);
  }
  
  engine::core::Config final_config(test_config_file_);
  EXPECT_EQ(final_config.target_fps_, 100);  // Last value set
}
