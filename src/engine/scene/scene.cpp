// Copyright Sylar129

#include "engine/scene/scene.h"

#include <algorithm>  // for std::remove_if

#include "engine/core/context.h"
#include "engine/object/game_object.h"
#include "engine/physics/physics_engine.h"
#include "engine/render/camera.h"
#include "engine/scene/scene_manager.h"
#include "engine/ui/ui_manager.h"
#include "log.h"

namespace engine::scene {

Scene::Scene(const std::string& name, engine::core::Context& context,
             engine::scene::SceneManager& scene_manager)
    : scene_name_(name),
      context_(context),
      scene_manager_(scene_manager),
      ui_manager_(std::make_unique<engine::ui::UIManager>()),
      is_initialized_(false) {
  ENGINE_TRACE("Scene '{}' created.", scene_name_);
}

Scene::~Scene() = default;

void Scene::Init() {
  is_initialized_ = true;
  ENGINE_TRACE("Scene '{}' initialized", scene_name_);
}

void Scene::Update(float delta_time) {
  if (!is_initialized_) {
    return;
  }

  context_.GetPhysicsEngine().Update(delta_time);

  context_.GetCamera().Update(delta_time);

  for (auto it = game_objects_.begin(); it != game_objects_.end();) {
    auto& object = *it;
    if (!object->IsNeedRemove()) {
      object->Update(delta_time, context_);
      ++it;
    } else {
      object->Clean();
      it = game_objects_.erase(it);
    }
  }

  ui_manager_->Update(delta_time, context_);

  ProcessPendingAdditions();
}

void Scene::Render() {
  if (!is_initialized_) {
    return;
  }
  for (const auto& obj : game_objects_) {
    obj->Render(context_);
  }

  ui_manager_->Render(context_);
}

void Scene::HandleInput() {
  if (!is_initialized_) {
    return;
  }

  if (ui_manager_->HandleInput(context_)) return;

  for (auto it = game_objects_.begin(); it != game_objects_.end();) {
    auto& object = *it;
    if (!object->IsNeedRemove()) {
      object->HandleInput(context_);
      ++it;
    } else {
      object->Clean();
      it = game_objects_.erase(it);
    }
  }
}

void Scene::Clean() {
  if (!is_initialized_) {
    return;
  }

  for (const auto& obj : game_objects_) {
    obj->Clean();
  }
  game_objects_.clear();

  is_initialized_ = false;
  ENGINE_TRACE("Scene '{}' clean finished", scene_name_);
}

void Scene::AddGameObject(
    std::unique_ptr<engine::object::GameObject>&& game_object) {
  if (game_object) {
    game_objects_.push_back(std::move(game_object));
  } else {
    ENGINE_WARN("Trying to add empty object to scene '{}'", scene_name_);
  }
}

void Scene::SafeAddGameObject(
    std::unique_ptr<engine::object::GameObject>&& game_object) {
  if (game_object) {
    pending_additions_.push_back(std::move(game_object));
  } else {
    ENGINE_WARN("Trying to add empty object to scene '{}'", scene_name_);
  }
}

void Scene::RemoveGameObject(engine::object::GameObject* game_object_ptr) {
  if (!game_object_ptr) {
    ENGINE_WARN("Trying to remove empty object ptr from scene '{}'",
                scene_name_);
    return;
  }

  auto it = std::remove_if(
      game_objects_.begin(), game_objects_.end(),
      [game_object_ptr](const std::unique_ptr<engine::object::GameObject>& p) {
        return p.get() == game_object_ptr;
      });

  if (it != game_objects_.end()) {
    game_object_ptr->Clean();
    game_objects_.erase(it, game_objects_.end());
    ENGINE_TRACE("Removing game object from scne '{}'", scene_name_);
  } else {
    ENGINE_WARN("Game object is not in the scene '{}'", scene_name_);
  }
}

void Scene::SafeRemoveGameObject(engine::object::GameObject* game_object_ptr) {
  game_object_ptr->SetNeedRemove(true);
}

engine::object::GameObject* Scene::FindGameObjectByName(
    const std::string& name) const {
  for (const auto& obj : game_objects_) {
    if (obj->GetName() == name) {
      return obj.get();
    }
  }
  return nullptr;
}

void Scene::ProcessPendingAdditions() {
  for (auto& game_object : pending_additions_) {
    AddGameObject(std::move(game_object));
  }
  pending_additions_.clear();
}

}  // namespace engine::scene
