#include "velox/input.h"

namespace vl {

bool Input::getMouseDown(MouseButton mouseButton) const {
  if (mouseButton == LMB)
    return m_mouseData & SDL_BUTTON_LMASK;
  else
    return m_mouseData & SDL_BUTTON_RMASK;
}

bool Input::wasMouseJustPressed(MouseButton mouseButton) const {
  if (mouseButton == LMB) {
    return (m_mouseData & SDL_BUTTON_LMASK) &&
           !(m_prevMouseData & SDL_BUTTON_LMASK);
  } else {
    return (m_mouseData & SDL_BUTTON_RMASK) &&
           !(m_prevMouseData & SDL_BUTTON_RMASK);
  }
}

bool Input::wasMouseJustReleased(MouseButton mouseButton) const {
  if (mouseButton == LMB) {
    return !(m_mouseData & SDL_BUTTON_LMASK) &&
           (m_prevMouseData & SDL_BUTTON_LMASK);
  } else {
    return !(m_mouseData & SDL_BUTTON_RMASK) &&
           (m_prevMouseData & SDL_BUTTON_RMASK);
  }
}

} // namespace vl
