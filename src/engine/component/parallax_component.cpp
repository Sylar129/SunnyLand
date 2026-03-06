#include "parallax_component.h"

#include <spdlog/spdlog.h>

#include "../core/context.h"
#include "../object/game_object.h"
#include "../render/camera.h"
#include "../render/renderer.h"
#include "../render/sprite.h"
#include "transform_component.h"

namespace engine::component {

ParallaxComponent::ParallaxComponent(const std::string& texture_id,
                                     const glm::vec2& scroll_factor,
                                     const glm::bvec2& repeat)
    : sprite_(engine::render::Sprite(texture_id)),  // 视差背景默认为整张图片
      scroll_factor_(scroll_factor),
      repeat_(repeat) {
  spdlog::trace("ParallaxComponent 初始化完成，纹理 ID: {}", texture_id);
}

void ParallaxComponent::Init() {
  if (!owner_) {
    spdlog::error("ParallaxComponent 初始化时，GameObject 为空。");
    return;
  }
  transform_ = owner_->GetComponent<TransformComponent>();
  if (!transform_) {
    spdlog::error(
        "ParallaxComponent 初始化时，GameObject 上没有找到 TransformComponent "
        "组件。");
    return;
  }
}

void ParallaxComponent::Render(engine::core::Context& context) {
  if (is_hidden_ || !transform_) {
    return;
  }
  // 直接调用视差滚动绘制函数
  context.getRenderer().DrawParallax(context.getCamera(), sprite_,
                                     transform_->GetPosition(), scroll_factor_,
                                     repeat_, transform_->GetScale());
}

}  // namespace engine::component