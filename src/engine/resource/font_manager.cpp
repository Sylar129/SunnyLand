// Copyright Sylar129

#include "font_manager.h"

#include <stdexcept>

#include "spdlog/spdlog.h"

namespace engine::resource {

FontManager::FontManager() {
  if (!TTF_WasInit() && !TTF_Init()) {
    throw std::runtime_error("FontManager 错误: TTF_Init 失败：" +
                             std::string(SDL_GetError()));
  }
  SPDLOG_TRACE("FontManager 构造成功。");
}

FontManager::~FontManager() {
  if (!fonts_.empty()) {
    SPDLOG_DEBUG("FontManager 不为空，调用 clearFonts 处理清理逻辑。");
    ClearFonts();
  }
  TTF_Quit();
  SPDLOG_TRACE("FontManager 析构成功。");
}

TTF_Font* FontManager::LoadFont(const std::string& file_path, int point_size) {
  if (point_size <= 0) {
    SPDLOG_ERROR("无法加载字体 '{}'：无效的点大小 {}。", file_path, point_size);
    return nullptr;
  }

  // 创建映射表的键
  FontKey key = {file_path, point_size};

  // 首先检查缓存
  auto it = fonts_.find(key);
  if (it != fonts_.end()) {
    return it->second.get();
  }

  // 缓存中不存在，则加载字体
  SPDLOG_DEBUG("正在加载字体：{} ({}pt)", file_path, point_size);
  TTF_Font* raw_font = TTF_OpenFont(file_path.c_str(), point_size);
  if (!raw_font) {
    SPDLOG_ERROR("加载字体 '{}' ({}pt) 失败：{}", file_path, point_size,
                 SDL_GetError());
    return nullptr;
  }

  // 使用 unique_ptr 存储到缓存中
  fonts_.emplace(key, std::unique_ptr<TTF_Font, SDLFontDeleter>(raw_font));
  SPDLOG_DEBUG("成功加载并缓存字体：{} ({}pt)", file_path, point_size);
  return raw_font;
}

TTF_Font* FontManager::GetFont(const std::string& file_path, int point_size) {
  FontKey key = {file_path, point_size};
  auto it = fonts_.find(key);
  if (it != fonts_.end()) {
    return it->second.get();
  }

  SPDLOG_WARN("字体 '{}' ({}pt) 不在缓存中，尝试加载。", file_path, point_size);
  return LoadFont(file_path, point_size);
}

void FontManager::UnloadFont(const std::string& file_path, int point_size) {
  FontKey key = {file_path, point_size};
  auto it = fonts_.find(key);
  if (it != fonts_.end()) {
    SPDLOG_DEBUG("卸载字体：{} ({}pt)", file_path, point_size);
    fonts_.erase(it);
  } else {
    SPDLOG_WARN("尝试卸载不存在的字体：{} ({}pt)", file_path, point_size);
  }
}

void FontManager::ClearFonts() {
  if (!fonts_.empty()) {
    SPDLOG_DEBUG("正在清理所有 {} 个缓存的字体。", fonts_.size());
    fonts_.clear();
  }
}

}  // namespace engine::resource