// Copyright Sylar129

#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

namespace game::data {

struct SessionData final {
  int current_health = 3;
  int max_health = 3;
  int current_score = 0;
  int high_score = 0;

  int level_health = 3;
  int level_score = 0;
  std::string map_path = "assets/maps/level1.tmj";
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SessionData, current_health,
                                                max_health, current_score,
                                                high_score, level_health,
                                                level_score, map_path);

inline bool saveToFile(const std::string& filename, const SessionData& data) {
  nlohmann::json j;
  try {
    j = data;

    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
      return false;
    }

    // 将 JSON 对象写入文件（使用4个空格进行美化输出）
    ofs << j.dump(4);
    ofs.close();  // 确保文件关闭

    return true;
  } catch (const std::exception& e) {
    return false;
  }
}

inline bool loadFromFile(const std::string& filename, SessionData& data) {
  try {
    // 打开文件进行读取
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
      // 如果存档文件不存在，这不一定是错误
      return false;
    }

    // 从文件解析 JSON 数据
    nlohmann::json j;
    ifs >> j;
    ifs.close();  // 读取完成后关闭文件

    data = j.get<SessionData>();

    return true;
  } catch (const std::exception& e) {
    return false;
  }
}

}  // namespace game::data