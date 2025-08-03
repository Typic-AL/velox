#include "velox/renderWindow.h"

namespace vl {

bool RenderWindow::init(const char *title, int w, int h) {

  if (!SDL_CreateWindowAndRenderer(title, w, h, SDL_WINDOW_HIGH_PIXEL_DENSITY,
                                   &m_window, &m_renderer)) {
    SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
    return false;
  }

  if (!TTF_Init()) {
    SDL_Log("Couldn't initialise SDL_ttf: %s\n", SDL_GetError());
    return false;
  }

  m_displayScale = SDL_GetWindowDisplayScale(m_window);

  m_width = w;
  m_height = h;
  m_refWidth = w;
  m_refHeight = h;

  return true;
}

void RenderWindow::clear(SDL_Color color) {
  SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
  SDL_RenderClear(m_renderer);
}

} // namespace vl
