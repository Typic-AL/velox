#pragma once

#include "../components/ui.h"
#include "velox/components/input.h"

namespace vl {

inline bool buttonPressed(Registry reg, UIButton &button) {
  Input &input = reg.getResource<Input>();
  if (wasKeyJustPressed(input, SDLK))
    float x, y;
  SDL_GetMouseState(&x, &y);

  bool xCheck = x <= button.rect.x + button.rect.w && x >= button.rect.x;
  bool yCheck = y >= button.rect.y + button.rect.h && x <= button.rect.y;

  return xCheck && yCheck;
}

inline void buttonSystem(Registry &reg) {
  for (auto [button] : reg.view<UIButton>()) {
    if (buttonPressed(reg, button))
      button.callback(reg);
  }
}

} // namespace vl
