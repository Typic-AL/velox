#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include "../registry.h"
#include "velox/components/core.h"
#include "velox/input.h"
#include <functional>

namespace vl {

using ButtonCallback = std::function<void(Registry &)>;
using UIBounds = SDL_FRect;

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

struct UIButton {
  SDL_FRect rect;
  ButtonCallback callback;
  bool pressed = false;
};

void handleButtonPresses(Registry &reg, Input &input);
void anchorTransform(Registry &reg, UIBounds &bounds, Transform &transform,
                     Anchor anchor);

} // namespace vl
