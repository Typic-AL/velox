#pragma once

#include "../core.h"
#include "velox/registry.h"

namespace vl {

struct UIBounds {
  float w, h;
};

enum class Anchor {
  TOP_LEFT,
  TOP_CENTER,
  TOP_RIGHT,
  CENTER_LEFT,
  CENTER,
  CENTER_RIGHT,
  BOTTOM_LEFT,
  BOTTOM_CENTER,
  BOTTOM_RIGHT
};

void anchorTransform(Registry &reg, UIBounds &bounds, Transform &transform,
                     Anchor anchor);

} // namespace vl
