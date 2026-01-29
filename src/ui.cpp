#include "velox/components/ui.h"
#include "velox/input.h"
#include "velox/registry.h"

namespace vl {

void handleButtonPresses(Registry &reg, Input &input) {
  for (auto &b : reg.view<UIButton>()) {
    UIButton &button = reg.get<UIButton>(b);

    if (!input.wasMouseJustReleased(LMB))
      return;

    glm::vec2 mousePos = input.getMousePos();
    SDL_FPoint mousePoint = {mousePos.x, mousePos.y};

    if (!(SDL_PointInRectFloat(&mousePoint, &button.rect)))
      return;

    button.callback(reg);
  }
}

} // namespace vl
