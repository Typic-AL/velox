#include "velox/components/ui/ui.h"
#include "velox/input.h"
#include "velox/registry.h"
#include "velox/renderWindow.h"
#include "velox/space.h"
#include "velox/time.h"
#include <algorithm>

namespace vl {

void handleButtonPresses(Registry &reg, Input &input) {
  for (auto [button, transform] : reg.view<Button, Transform>()) {

    glm::vec2 mousePos = input.getMousePos();
    if (button.space == Space::WORLD)
      mousePos = screenToWorld(reg, mousePos);

    SDL_FPoint mousePoint = {mousePos.x, mousePos.y};
    SDL_FRect rect = {transform.pos.x, transform.pos.y, button.w, button.h};

    if (!(SDL_PointInRectFloat(&mousePoint, &rect)))
      continue;

    if (input.wasMouseJustPressed(LMB)) {
      button.pressed = true;
      continue;
    }

    if (!button.pressed)
      continue;

    if (!input.wasMouseJustReleased(LMB))
      continue;

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

void updateProgressBars(Registry &reg) {
  for (auto [bar] : reg.view<ProgressBar>()) {
    bar.value = std::clamp(bar.value, 0.0f, 1.0f);
    float diff = bar.value - bar.displayValue;
    bar.displayValue += diff * std::min(1.0f, bar.speed * Time::deltaTime);
  }
}

} // namespace vl
