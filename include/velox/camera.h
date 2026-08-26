#pragma once

#include "velox/entity.h"

#include <glm/vec2.hpp>
#include <limits>

namespace vl {

class Registry;

constexpr Entity NO_ENTITY = std::numeric_limits<Entity>::max();

struct Camera {
  glm::vec2 pos{};
  glm::vec2 followOffset{};
  Entity target = NO_ENTITY;
  glm::vec2 deadzone{};   // half-extents in world units
  float smoothing = 8.0f; // lerp speed; 0 = instant snap

  // Optional world bounds clamping; set max < min to disable
  glm::vec2 boundsMin{0, 0};
  glm::vec2 boundsMax{-1, -1};
};

void updateCamera(Registry &reg);

} // namespace vl
