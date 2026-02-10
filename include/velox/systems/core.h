#pragma once

#include "velox/components/core.h"
#include "velox/entity.h"
#include "velox/registry.h"

namespace vl {

inline void applyVelocity(Registry &reg) {
  for (auto e : reg.view<Rigidbody, Transform>()) {
    Transform &transform = reg.get<Transform>(e);
    Rigidbody &rigidbody = reg.get<Rigidbody>(e);
    transform.pos += rigidbody.vel;
  }
}

} // namespace vl
