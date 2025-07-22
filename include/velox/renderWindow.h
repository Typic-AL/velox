#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "glm/glm.hpp"
#include "util.h"

#include "resourceIDs.h"

namespace vl {

struct Tilemap;

struct RenderCommand {
  SDL_Texture *texture;
  SDL_FRect dstRect;
  SDL_FRect srcRect;
  int zIndex = 0;
  bool useRenderScale = true;
  bool isUi = false;
  RenderCommand(SDL_Texture *tex, SDL_FRect dst, SDL_FRect src, int zIndex,
                bool isUi, bool useRenderScale) {
    texture = tex;
    dstRect = dst;
    srcRect = src;
    this->zIndex = zIndex;
    this->isUi = isUi;
    this->useRenderScale = useRenderScale;
  }
};

class RenderWindow {
private:
  SDL_Window *m_window = nullptr;
  SDL_Renderer *m_renderer = nullptr;

  int m_width, m_height;
  int m_refWidth, m_refHeight;
  SDL_RendererLogicalPresentation m_presentationMode;
  int m_uiLayer = 100;
  float m_displayScale = 1;

  std::vector<RenderCommand> m_renderQueue;
  std::unordered_map<
      TextureID, std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>>
      m_texCache;
  std::unordered_map<FontID,
                     std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>>
      m_fontCache;

  static void destroyTexture(SDL_Texture *tex) {
    if (tex)
      SDL_DestroyTexture(tex);
  }
  static void closeFont(TTF_Font *font) {
    if (font)
      TTF_CloseFont(font);
  }

  bool m_useIntDrawing = false;

public:
  RenderWindow() {}
  ~RenderWindow() {}

  bool init(const char *title, int w, int h);
  void clear(SDL_Color color);
  void endDrawing();

  void drawTexture(SDL_FRect src, SDL_FRect dst, TextureID id, int zIndex,
                   bool isUi);
  void drawText(FontID id, std::string text, glm::vec2 pos, int size,
                SDL_Color color, int zIndex, bool centered, bool pixelFont,
                bool useRenderScale);
  void drawRect(SDL_FRect rect, SDL_Color color, int zIndex, float alpha);
  void generateFontTexture(FontID id, int size);
  void generateTextTexture(FontID id, std::string text);

  void clearTexCache() {
    m_texCache.clear();
    m_fontCache.clear();
  }

  void setReferenceResolution(int w, int h,
                              SDL_RendererLogicalPresentation mode) {
    SDL_SetRenderLogicalPresentation(m_renderer, w, h, mode);
    m_refWidth = w;
    m_refHeight = h;
    m_presentationMode = mode;
  }

  void renderTilemap(SDL_Texture *tileset, const Tilemap &map);

  int getScreenWidth() { return m_width; }
  int getScreenHeight() { return m_height; }

  void enableIntDrawing() { m_useIntDrawing = true; }
  void enableVsync() { SDL_SetRenderVSync(m_renderer, 1); }

  SDL_Window *getWin() { return m_window; }
  SDL_Renderer *getRen() { return m_renderer; }
};
} // namespace vl
