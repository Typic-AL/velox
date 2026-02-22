#pragma once

#include "../time.h"
#include "velox/components/core.h"
#include "velox/entity.h"
#include "velox/registry.h"
#include <iostream>
namespace vl {

inline void applyVelocity(Registry &reg) {
  for (auto e : reg.view<Rigidbody, Transform>()) {
    Transform &transform = reg.get<Transform>(e);
    Rigidbody &rigidbody = reg.get<Rigidbody>(e);
    transform.prevPos = transform.pos;
    transform.pos += rigidbody.vel;
  }
}

inline void interpolatePosition(Registry &reg) {
  for (auto e : reg.view<Rigidbody, Transform>()) {
    Transform &transform = reg.get<Transform>(e);
    transform.lPos = glm::mix(transform.prevPos, transform.pos, Time::alpha);
  }
}

} // namespace vl
