#pragma once

#include "velox/input.h"
#include "velox/registry.h"
#include "velox/space.h"
#include <SDL3/SDL.h>
#include <functional>

namespace vl {

using ButtonCallback = std::function<void(Registry &)>;

struct Button {
  float w, h;
  ButtonCallback callback;
  bool pressed = false;
  Space space = Space::SCREEN;
};

void handleButtonPresses(Registry &reg, Input &input);
} // namespace vl
