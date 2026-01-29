#pragma once

#include "glm/glm.hpp"
#include "velox/renderWindow.h"
#include <SDL3/SDL.h>

#include <iostream>
#include <vector>

namespace vl {

enum class MouseButton { LEFT, RIGHT };

inline constexpr MouseButton LMB = MouseButton::LEFT;
inline constexpr MouseButton RMB = MouseButton::RIGHT;

class Input {
private:
  int m_numKeys;
  const bool *m_keyboard;
  std::vector<bool> m_prevKeyboard;
  SDL_MouseButtonFlags m_mouseData;
  SDL_MouseButtonFlags m_prevMouseData;
  glm::vec2 m_mousePos;
  RenderWindow &window;

public:
  Input(RenderWindow &window) : window(window) {
    m_keyboard = SDL_GetKeyboardState(&m_numKeys);

    m_prevKeyboard.resize(m_numKeys, false);
  }

  bool wasKeyJustPressed(SDL_Scancode key) const {
    return m_keyboard[key] && !m_prevKeyboard[key];
  }
  bool wasKeyJustReleased(SDL_Scancode key) const {
    return m_prevKeyboard[key] && !m_keyboard[key];
  }

  void reset() {
    std::copy(m_keyboard, m_keyboard + m_numKeys, m_prevKeyboard.begin());
    m_prevMouseData = m_mouseData;
  }

  void updateMouseData() {
    m_mouseData = SDL_GetMouseState(&m_mousePos.x, &m_mousePos.y);
    m_mousePos.x /= (float)window.getScreenWidth() / window.getReferenceWidth();
    m_mousePos.y /=
        (float)window.getScreenHeight() / window.getReferenceHeight();
  }

  bool getKeyDown(SDL_Scancode key) { return m_keyboard[key]; }

  bool getMouseDown(MouseButton mouseButton) const;
  bool wasMouseJustPressed(MouseButton mouseButton) const;
  bool wasMouseJustReleased(MouseButton mouseButton) const;
  glm::vec2 getMousePos() { return m_mousePos; }
};

} // namespace vl
