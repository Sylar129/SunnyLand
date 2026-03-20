// Copyright Sylar129

#include "engine/scene/scene.h"

#include "engine/core/context.h"
#include "engine/core/game_state.h"
#include "engine/render/camera.h"
#include "engine/scene/scene_manager.h"
#include "engine/system/physics_engine.h"
#include "engine/ui/ui_manager.h"
#include "utils/log.h"

namespace engine::scene {

Scene::Scene(const std::string& name, core::Context& context,
             SceneManager& scene_manager)
    : scene_name_(name),
      context_(context),
      scene_manager_(scene_manager),
      ui_manager_(std::make_unique<ui::UIManager>()),
      is_initialized_(false) {
  ENGINE_LOG_TRACE("Scene '{}' created.", scene_name_);
}

Scene::~Scene() = default;

void Scene::Init() {
  is_initialized_ = true;
  ENGINE_LOG_TRACE("Scene '{}' initialized", scene_name_);
}

void Scene::Update(float delta_time) {
  if (!is_initialized_) {
    return;
  }

  if (context_.GetGameState().IsPlaying()) {
    context_.GetPhysicsEngine().Update(delta_time);
    context_.GetCamera().Update(delta_time);
  }

  ui_manager_->Update(delta_time, context_);
}

void Scene::Render() {
  if (!is_initialized_) {
    return;
  }

  ui_manager_->Render(context_);
}

void Scene::HandleInput() {
  if (!is_initialized_) {
    return;
  }

  if (ui_manager_->HandleInput(context_)) return;
}

void Scene::Clean() {
  if (!is_initialized_) {
    return;
  }

  is_initialized_ = false;
  ENGINE_LOG_TRACE("Scene '{}' clean finished", scene_name_);
}

}  // namespace engine::scene
