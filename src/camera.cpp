#include "velox/camera.h"

#include "velox/components/core.h"
#include "velox/registry.h"
#include "velox/time.h"

#include <cmath>
#include <glm/common.hpp>

namespace vl {

void updateCamera(Registry &reg) {
  auto it = reg.tryGetResource<Camera>();
  if (!it)
    return;

  Camera &cam = *it;
  if (cam.target == NO_ENTITY || !reg.has<Transform>(cam.target))
    return;

  glm::vec2 targetPos = reg.get<Transform>(cam.target).lPos;
  glm::vec2 delta = targetPos - cam.pos;

  // Only move camera when entity exits the deadzone box
  if (std::abs(delta.x) > cam.deadzone.x)
    delta.x -= glm::sign(delta.x) * cam.deadzone.x;
  else
    delta.x = 0;

  if (std::abs(delta.y) > cam.deadzone.y)
    delta.y -= glm::sign(delta.y) * cam.deadzone.y;
  else
    delta.y = 0;

  float t = cam.smoothing > 0.0f
                ? glm::clamp(cam.smoothing * Time::deltaTime, 0.0f, 1.0f)
                : 1.0f;
  cam.pos += delta * t;

  // Optional world bounds clamp
  if (cam.boundsMax.x >= cam.boundsMin.x)
    cam.pos.x = glm::clamp(cam.pos.x, cam.boundsMin.x, cam.boundsMax.x);
  if (cam.boundsMax.y >= cam.boundsMin.y)
    cam.pos.y = glm::clamp(cam.pos.y, cam.boundsMin.y, cam.boundsMax.y);
}

} // namespace vl
