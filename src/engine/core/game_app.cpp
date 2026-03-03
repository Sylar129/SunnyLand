// Copyright Sylar129

#include "engine/core/game_app.h"

#include <memory>

#include "SDL3/SDL.h"
#include "engine/core/time.h"
#include "engine/render/camera.h"
#include "engine/render/renderer.h"
#include "engine/resource/resource_manager.h"
#include "log.h"

namespace engine::core {

GameApp::GameApp() {}

GameApp::~GameApp() {
  if (is_running_) {
    ENGINE_WARN("GameApp is still running when dtor!");
    Close();
  }
}

void GameApp::Run() {
  if (!Init()) {
    ENGINE_ERROR("Failed to init game!");
    return;
  }
  time_->SetTargetFps(144);

  while (is_running_) {
    time_->Update();
    float delta_time = time_->GetDeltaTime();

    HandleEvents();
    Update(delta_time);
    Render();
  }

  Close();
}

bool GameApp::Init() {
  ENGINE_TRACE("Init GameApp ...");
  if (!InitSDL()) return false;
  if (!InitTime()) return false;
  if (!InitResourceManager()) return false;
  if (!initRenderer()) return false;
  if (!initCamera()) return false;

  is_running_ = true;
  return true;
}

bool GameApp::InitSDL() {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    ENGINE_ERROR("Failed to init SDL! Error: {}", SDL_GetError());
    return false;
  }

  window_ = SDL_CreateWindow("SunnyLand", 1280, 720, SDL_WINDOW_RESIZABLE);
  if (window_ == nullptr) {
    ENGINE_ERROR("Failed to create window! Error: {}", SDL_GetError());
    return false;
  }

  sdl_renderer_ = SDL_CreateRenderer(window_, nullptr);
  if (sdl_renderer_ == nullptr) {
    ENGINE_ERROR("Failed to create renderer! Error: {}", SDL_GetError());
    return false;
  }
  SDL_SetRenderLogicalPresentation(sdl_renderer_, 640, 360,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);
  return true;
}

bool GameApp::initRenderer() {
  try {
    renderer_ = std::make_unique<engine::render::Renderer>(
        sdl_renderer_, resource_manager_.get());
  } catch (const std::exception& e) {
    ENGINE_ERROR("初始化渲染器失败: {}", e.what());
    return false;
  }
  ENGINE_TRACE("渲染器初始化成功。");
  return true;
}

bool GameApp::initCamera() {
  try {
    camera_ = std::make_unique<engine::render::Camera>(glm::vec2(640, 360));
  } catch (const std::exception& e) {
    ENGINE_ERROR("初始化相机失败: {}", e.what());
    return false;
  }
  ENGINE_TRACE("相机初始化成功。");
  return true;
}

bool GameApp::InitTime() {
  time_ = std::make_unique<Time>();
  return true;
}

bool GameApp::InitResourceManager() {
  resource_manager_ =
      std::make_unique<engine::resource::ResourceManager>(sdl_renderer_);
  return true;
}

void GameApp::HandleEvents() {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_EVENT_QUIT) {
      is_running_ = false;
    }
  }
}

void GameApp::Update(float delta_time) { testCamera(); }

void GameApp::Render() {
  // 1. 清除屏幕
  renderer_->clearScreen();

  // 2. 具体渲染代码
  testRenderer();

  // 3. 更新屏幕显示
  renderer_->present();
}

void GameApp::Close() {
  ENGINE_TRACE("Closing GameApp...");
  resource_manager_.reset();

  if (sdl_renderer_ != nullptr) {
    SDL_DestroyRenderer(sdl_renderer_);
    sdl_renderer_ = nullptr;
  }
  if (window_ != nullptr) {
    SDL_DestroyWindow(window_);
    window_ = nullptr;
  }
  SDL_Quit();
  is_running_ = false;
}

void GameApp::testRenderer() {
  engine::render::Sprite sprite_world("assets/textures/Actors/frog.png");
  engine::render::Sprite sprite_ui("assets/textures/UI/buttons/Start1.png");
  engine::render::Sprite sprite_parallax("assets/textures/Layers/back.png");

  static float rotation = 0.0f;
  rotation += 0.1f;

  // 注意渲染顺序
  renderer_->drawParallax(*camera_, sprite_parallax, glm::vec2(100, 100),
                          glm::vec2(0.5f, 0.5f), glm::bvec2(true, false));
  renderer_->drawSprite(*camera_, sprite_world, glm::vec2(200, 200),
                        glm::vec2(1.0f, 1.0f), rotation);
  renderer_->drawUISprite(sprite_ui, glm::vec2(100, 100));
}

void GameApp::testCamera() {
  auto key_state = SDL_GetKeyboardState(nullptr);
  if (key_state[SDL_SCANCODE_UP]) camera_->move(glm::vec2(0, -1));
  if (key_state[SDL_SCANCODE_DOWN]) camera_->move(glm::vec2(0, 1));
  if (key_state[SDL_SCANCODE_LEFT]) camera_->move(glm::vec2(-1, 0));
  if (key_state[SDL_SCANCODE_RIGHT]) camera_->move(glm::vec2(1, 0));
}

}  // namespace engine::core
