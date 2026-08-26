#pragma once

#include "velox/entity.h"
#include <SDL3/SDL.h>
#include <functional>
#include <glm/vec2.hpp>
#include <unordered_map>

namespace vl {

using CollisionCallback = std::function<void(Entity, Entity)>;

struct Collider {
  float width;
  float height;
  glm::vec2 offset{0, 0};
  CollisionCallback onCollide = {};
  int layer = 0;
  int mask = ~0;
  bool isTrigger = false;
  CollisionCallback onEnter = {};
  CollisionCallback onExit = {};
};

inline int createLayerMask(const std::vector<int> &layers, bool exclude = false) {
  int mask = exclude ? ~0 : 0;
  for (int layer : layers) {
    if (exclude)
      mask &= ~(1 << layer);
    else
      mask |= (1 << layer);
  }
  return mask;
}

} // namespace vl
