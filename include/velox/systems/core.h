#pragma once

#include "velox/components/core.h"
#include "velox/entity.h"
#include "velox/registry.h"

namespace vl {

inline void syncPositions(Registry &reg) {
  for (auto e : reg.view<Transform, PositionUpdater>()) {
    PositionUpdater &updater = reg.get<PositionUpdater>(e);
    Transform &transform = reg.get<Transform>(e);
    for (auto &[x, y] : updater.syncTargets) {
      x = transform.pos.x;
      y = transform.pos.y;
    }
  }
}

inline void syncPositionWithTransform(Registry &reg, Entity e, float &x,
                                      float &y) {
  if (!reg.hasComponent<PositionUpdater>(e)) {
    reg.addComponent(e, PositionUpdater{});
  }
  PositionUpdater &updater = reg.get<PositionUpdater>(e);
  updater.syncTargets.push_back({x, y});
}

inline void applyVelocity(Registry &reg) {
  for (auto e : reg.view<Rigidbody, Transform>()) {
    Transform &transform = reg.get<Transform>(e);
    Rigidbody &rigidbody = reg.get<Rigidbody>(e);
    transform.pos += rigidbody.vel;
  }
}

} // namespace vl
