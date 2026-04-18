#pragma once

#include "../time.h"
#include "velox/components/core.h"
#include "velox/entity.h"
#include "velox/registry.h"
#include <iostream>
namespace vl {

inline void applyVelocity(Registry &reg) {
  for (auto [rb, t] : reg.view<Rigidbody, Transform>()) {
    t.prevPos = t.pos;
    t.pos += rb.vel;
  }
}

inline void interpolatePosition(Registry &reg) {
  for (auto [rb, t] : reg.view<Rigidbody, Transform>()) {
    t.lPos = glm::mix(t.prevPos, t.pos, Time::alpha);
  }
}

} // namespace vl
