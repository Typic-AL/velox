#include "velox/space.h"
#include "velox/camera.h"
#include "velox/registry.h"
#include "velox/renderWindow.h"

namespace vl {

glm::vec2 cameraOffset(Registry &reg) {
  Camera *cam = reg.tryGetResource<Camera>();
  if (!cam)
    return {0, 0};

  RenderWindow &window = reg.getResource<RenderWindow>();
  float refW = static_cast<float>(window.getReferenceWidth());
  float refH = static_cast<float>(window.getReferenceHeight());
  return cam->pos + cam->followOffset - glm::vec2(refW * 0.5f, refH * 0.5f);
}

glm::vec2 worldToScreen(Registry &reg, glm::vec2 world) {
  return world - cameraOffset(reg);
}

glm::vec2 screenToWorld(Registry &reg, glm::vec2 screen) {
  return screen + cameraOffset(reg);
}

} // namespace vl
