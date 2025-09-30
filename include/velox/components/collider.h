#pragma once

#include <SDL3/SDL.h>
#include <functional>
#include <unordered_map>
#include "velox/entity.h"

namespace vl {

using CollisionCallback = std::function<void(Entity, Entity)>;

struct Collider {
  SDL_FRect rect;
  CollisionCallback onCollide = {};
  int layer = 0;
  int mask = ~0;
  bool isTrigger = false;
  CollisionCallback onEnter = {};
  CollisionCallback onExit = {};
};

inline int createlayerMask(std::vector<int> layers, bool exclude = false) {
    int mask = exclude ? 0 : ~0;
    for (int layer : layers) {
        mask |= ( (exclude ? 0 : 1) << layer);
    }
    return mask;
}

}
