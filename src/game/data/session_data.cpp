// Copyright Sylar129

#include "game/data/session_data.h"

#include <fstream>

#include "log.h"

namespace game::data {

bool Session::SaveToFile(const std::string& filename) const {
  try {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
      return false;
    }

    nlohmann::json j = data_;
    ofs << j.dump(4);
    ofs.close();
  } catch (const std::exception& e) {
    GAME_ERROR("Save to {} Failed, e: {}", filename, e.what());
    return false;
  }
  return true;
}

bool Session::LoadFromFile(const std::string& filename) {
  try {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
      return false;
    }

    nlohmann::json j;
    ifs >> j;
    ifs.close();

    data_ = j.get<Session::Data>();
  } catch (const std::exception& e) {
    GAME_ERROR("Load from {} Failed, e: {}", filename, e.what());
    return false;
  }
  return true;
}

}  // namespace game::data
