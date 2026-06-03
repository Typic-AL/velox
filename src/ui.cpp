#include "velox/components/core.h"
#include "velox/components/tintable.h"
#include "velox/components/ui/ui.h"
#include "velox/input.h"
#include "velox/registry.h"
#include "velox/renderWindow.h"
#include "velox/space.h"
#include "velox/time.h"
#include <algorithm>

namespace vl {

static void applyButtonTint(Registry &reg, Entity e, SDL_Color tint) {
  if (reg.has<Tintable>(e))
    reg.get<Tintable>(e).tint = tint;
}

void handleButtonPresses(Registry &reg, Input &input) {
  for (auto [button, transform, entity] :
       reg.view<Button, Transform>().withEntity()) {

    glm::vec2 mousePos = input.getMousePos();
    if (button.space == Space::WORLD)
      mousePos = screenToWorld(reg, mousePos);

    SDL_FPoint mousePoint = {mousePos.x, mousePos.y};
    SDL_FRect rect = {transform.pos.x, transform.pos.y, button.w, button.h};

    bool hovered = SDL_PointInRectFloat(&mousePoint, &rect);
    bool justPressed = input.wasMouseJustPressed(LMB);
    bool justReleased = input.wasMouseJustReleased(LMB);

    ButtonState next = button.state;

    if (!hovered) {
      if (justReleased)
        next = ButtonState::IDLE;
      else if (button.state != ButtonState::PRESSED)
        next = ButtonState::IDLE;
    } else if (justPressed) {
      next = ButtonState::PRESSED;
    } else if (justReleased && button.state == ButtonState::PRESSED) {
      next = ButtonState::HOVERED;
    } else if (button.state == ButtonState::IDLE) {
      next = ButtonState::HOVERED;
    }

    bool clicked = hovered && justReleased && button.state == ButtonState::PRESSED;

    if (next != button.state) {
      button.state = next;
      auto tint = [&]() -> SDL_Color {
        switch (next) {
        case ButtonState::HOVERED: return button.hoverTint;
        case ButtonState::PRESSED: return button.pressedTint;
        default:                   return button.idleTint;
        }
      }();
      applyButtonTint(reg, entity, tint);
    }

    if (clicked)
      button.callback(reg);
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
