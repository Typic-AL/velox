#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace vl {


class RenderWindow {
private:
  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;

  int m_width, m_height;
  int m_refWidth, m_refHeight;
  SDL_RendererLogicalPresentation m_presentationMode;
  int m_uiLayer = 100;
  float m_displayScale = 1;

  bool m_useIntDrawing = false;

public:
  RenderWindow() {}
  ~RenderWindow() {
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
  }

  bool init(const char *title, int w, int h);
  void clear(SDL_Color color);
  void present() {SDL_RenderPresent(m_renderer);}

  void setReferenceResolution(int w, int h,
                              SDL_RendererLogicalPresentation mode) {
    SDL_SetRenderLogicalPresentation(m_renderer, w, h, mode);
    m_refWidth = w;
    m_refHeight = h;
    m_presentationMode = mode;
  }

  int getReferenceWidth() const { return m_refWidth; }

  int getReferenceHeight() const { return m_refHeight; }

  SDL_RendererLogicalPresentation getPresentationMode() const {
    return m_presentationMode;
  }

  int getScreenWidth() { return m_width; }
  int getScreenHeight() { return m_height; }

  void enableIntDrawing() { m_useIntDrawing = true; }
  void enableVsync() { SDL_SetRenderVSync(m_renderer, 1); }

  SDL_Window *getWin() { return m_window; }
  SDL_Renderer *getRen() { return m_renderer; }
};
} // namespace vl
