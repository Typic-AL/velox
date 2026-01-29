#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include "../registry.h"
#include "velox/input.h"
#include <functional>

namespace vl {

using ButtonCallback = std::function<void(Registry &)>;

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

struct UITransform {
  Anchor anchor;
  glm::vec2 offset;
};

struct UIButton {
  SDL_FRect rect;
  ButtonCallback callback;
};

void handleButtonPresses(Registry &reg, Input &input);

} // namespace vl
