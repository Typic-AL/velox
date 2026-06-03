#pragma once

#include "velox/input.h"
#include "velox/registry.h"
#include "velox/space.h"
#include <SDL3/SDL.h>
#include <functional>

namespace vl {

using ButtonCallback = std::function<void(Registry &)>;

enum class ButtonState { IDLE, HOVERED, PRESSED };

struct Button {
  float w, h;
  ButtonCallback callback;
  ButtonState state = ButtonState::IDLE;
  Space space = Space::SCREEN;
  SDL_Color idleTint = {255, 255, 255, 255};
  SDL_Color hoverTint = {255, 255, 255, 255};
  SDL_Color pressedTint = {255, 255, 255, 255};
};

void handleButtonPresses(Registry &reg, Input &input);
} // namespace vl
