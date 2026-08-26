#include "velox/systems/core.h"

#include "velox/components/core.h"
#include "velox/registry.h"
#include "velox/time.h"

#include <glm/common.hpp>

namespace vl {

void applyVelocity(Registry &reg) {
  for (auto [rb, t] : reg.view<Rigidbody, Transform>()) {
    t.prevPos = t.pos;
    t.pos += rb.vel;
  }
}

void interpolatePosition(Registry &reg) {
  for (auto [rb, t] : reg.view<Rigidbody, Transform>()) {
    t.lPos = glm::mix(t.prevPos, t.pos, Time::alpha);
  }
}

} // namespace vl
