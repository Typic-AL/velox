#include "velox/components/ui.h"
#include "velox/input.h"
#include "velox/registry.h"

namespace vl {

void handleButtonPresses(Registry &reg, Input &input) {
  for (auto &b : reg.view<UIButton>()) {
    UIButton &button = reg.get<UIButton>(b);

    glm::vec2 mousePos = input.getMousePos();
    SDL_FPoint mousePoint = {mousePos.x, mousePos.y};

    if (!(SDL_PointInRectFloat(&mousePoint, &button.rect)))
      continue;

    if (input.wasMouseJustPressed(LMB)) {
      button.pressed = true;
      continue;
    }

    if (!button.pressed)
      continue;

    if (!input.wasMouseJustReleased(LMB))
      return;

    button.callback(reg);
    button.pressed = false;
  }
}

} // namespace vl
