// Copyright Sylar129

#pragma once

#include <string>

#include "nlohmann/json.hpp"

namespace game::data {

class Session {
 public:
  struct Data {
    int current_health = 3;
    int max_health = 3;
    int current_score = 0;
    int high_score = 0;

    int level_health = 3;
    int level_score = 0;
    std::string map_path = "assets/maps/level1.tmj";
  };

  std::string GetCurrentMapPath() const { return data_.map_path; }
  int GetCurrentHealth() const { return data_.current_health; }
  int GetMaxHealth() const { return data_.max_health; }
  int GetCurrentScore() const { return data_.current_score; }
  int GetHighScore() const { return data_.high_score; }
  bool GetIsWin() const { return is_win_; }

  void SetNextMapPath(const std::string& path) { data_.map_path = path; }
  void SetCurrentHealth(int health) { data_.current_health = health; }
  void SetCurrentScore(int score) { data_.current_score = score; }
  void AddScore(int score) { data_.current_score += score; }
  void SetIsWin(bool is_win) { is_win_ = is_win; }

  bool SaveToFile(const std::string& filename) const;
  bool LoadFromFile(const std::string& filename);

 private:
  Data data_;
  bool is_win_ = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Session::Data, current_health,
                                                max_health, current_score,
                                                high_score, level_health,
                                                level_score, map_path);

}  // namespace game::data
