#include "velox/components/ui.h"
#include "velox/input.h"
#include "velox/registry.h"
#include "velox/renderWindow.h"

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

void anchorTransform(Registry &reg, UIBounds &bounds, Transform &transform,
                     Anchor anchor) {
  RenderWindow &window = reg.getResource<RenderWindow>();
  float sw = window.getReferenceWidth();
  float sh = window.getReferenceHeight();
  float hw = bounds.w / 2.0f;
  float hh = bounds.h / 2.0f;

  switch (anchor) {
  case Anchor::TOP_LEFT:
    break;
  case Anchor::TOP_CENTER:
    transform.pos.x += sw / 2.0f - hw;
    break;
  case Anchor::TOP_RIGHT:
    transform.pos.x += sw - bounds.w;
    break;
  case Anchor::CENTER_LEFT:
    transform.pos.y += sh / 2.0f - hh;
    break;
  case Anchor::CENTER:
    transform.pos.x += sw / 2.0f - hw;
    transform.pos.y += sh / 2.0f - hh;
    break;
  case Anchor::CENTER_RIGHT:
    transform.pos.x += sw - bounds.w;
    transform.pos.y += sh / 2.0f - hh;
    break;
  case Anchor::BOTTOM_LEFT:
    transform.pos.y += sh - bounds.h;
    break;
  case Anchor::BOTTOM_CENTER:
    transform.pos.x += sw / 2.0f - hw;
    transform.pos.y += sh - bounds.h;
    break;
  case Anchor::BOTTOM_RIGHT:
    transform.pos.x += sw - bounds.w;
    transform.pos.y += sh - bounds.h;
    break;
  }
}

} // namespace vl
