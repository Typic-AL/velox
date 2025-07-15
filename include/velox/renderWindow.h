#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Math.h"
#include "util.h"

namespace vl {

struct Tilemap;

struct RenderCommand {
  SDL_Texture *texture;
  SDL_FRect dstRect;
  SDL_FRect srcRect;
  Vec2 pos;
  int zIndex = 0;
  bool useRenderScale = true;
  bool isUi = false;
  RenderCommand(SDL_Texture *tex, SDL_FRect dst, SDL_FRect src, Vec2 pos,
                int zIndex, bool isUi, bool useRenderScale) {
    texture = tex;
    dstRect = dst;
    srcRect = src;
    this->pos = pos;
    this->zIndex = zIndex;
    this->isUi = isUi;
    this->useRenderScale = useRenderScale;
  }
};

class RenderWindow {
private:
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;

  int width, height;
  int refWidth, refHeight;
  SDL_RendererLogicalPresentation presentationMode;
  int uiLayer = 100;
  float displayScale = 1;

  std::vector<RenderCommand> renderQueue;
  std::unordered_map<std::pair<TTF_Font *, std::string>, SDL_Texture *,
                     PairHash<TTF_Font *, std::string>>
      ttfTexCache;
  std::unordered_map<std::pair<std::string, int>, TTF_Font *,
                     PairHash<std::string, int>>
      ttfFontCache;
  std::unordered_map<const char *, SDL_Texture *> texCache;

  bool useIntDrawing = false;

public:
  RenderWindow() { g_window = this; }
  ~RenderWindow() {
    for (const auto &tex : ttfTexCache)
      SDL_DestroyTexture(tex.second);

    for (const auto &tex : texCache)
      SDL_DestroyTexture(tex.second);

    for (const auto &fnt : ttfFontCache)
      TTF_CloseFont(fnt.second);
  }

  bool init(const char *title, int w, int h);
  void beginDrawing(SDL_Color color);
  void endDrawing();
  void render(Vec2 pos, SDL_Texture *tex);

  void addToQueue(const SDL_FRect &src, const Vec2 &pos, SDL_Texture *tex,
                  const int &zIndex, bool isUi = false,
                  bool useRenderScale = true);
  void addToQueueDirect(const SDL_FRect &src, const SDL_FRect &dst,
                        SDL_Texture *tex, const int &zIndex, bool isUi = false,
                        bool useRenderScale = true);
  void addTextToQueue(const char *fntPath, std::string text, Vec2 pos, int size,
                      SDL_Color color, int zIndex, bool centered = false,
                      bool pixelFont = false, bool useRenderScale = true);

  void addRectToQueue(const Vec2 &pos, float width, float height,
                      const SDL_Color &color, const int &zIndex,
                      float alpha = 255);
  void addRectToQueueDirect(const SDL_FRect &rect, const SDL_Color &color,
                            const int &zIndex, float alpha = 255);

  void clearTexCache() {
    ttfTexCache.clear();
    ttfFontCache.clear();
    texCache.clear();
  }

  void setReferenceResolution(int w, int h,
                              SDL_RendererLogicalPresentation mode) {
    SDL_SetRenderLogicalPresentation(renderer, w, h, mode);
    refWidth = w;
    refHeight = h;
    presentationMode = mode;
  }

  void renderTilemap(SDL_Texture *tileset, const Tilemap &map);

  int getScreenWidth() { return width; }
  int getScreenHeight() { return height; }

  void enableIntDrawing() { useIntDrawing = true; }
  void enableVsync() { SDL_SetRenderVSync(renderer, 1); }

  SDL_Texture *loadTexture(const char *filepath) {
    if (texCache.find(filepath) != texCache.end())
      return texCache[filepath];

    SDL_Texture *tex = IMG_LoadTexture(renderer, filepath);
    if (!tex) {
      std::cerr << "[RenderWindow] Failed to load texture: " << SDL_GetError()
                << "\n";
    }

    texCache[filepath] = tex;

    return tex;
  }

  SDL_Window *getWin() { return window; }
  SDL_Renderer *getRen() { return renderer; }
};
} // namespace vl
