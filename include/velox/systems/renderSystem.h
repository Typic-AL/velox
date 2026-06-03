#pragma once

#include "velox/components/ui/ui.h"
#include "velox/space.h"
#include <vector>

namespace vl {

class RenderWindow;
class Registry;
class AssetManager;

struct RenderCommand {
  SDL_Texture *tex = nullptr;
  SDL_FRect dstRect;
  SDL_FRect srcRect;
  SDL_Color color = {255, 255, 255, 255};
  int zIndex = 0;
  bool useRenderScale = true;
  Space space = Space::WORLD;
  bool isRect = false;

  RenderCommand(SDL_Texture *tex, SDL_FRect dst, SDL_FRect src, int zIndex,
                Space space, bool useRenderScale,
                SDL_Color tint = {255, 255, 255, 255}) {
    this->tex = tex;
    dstRect = dst;
    srcRect = src;
    this->zIndex = zIndex;
    this->space = space;
    this->useRenderScale = useRenderScale;
    this->color = tint;
  }

  RenderCommand(SDL_FRect dst, SDL_Color color, int zIndex, Space space) {
    dstRect = dst;
    this->color = color;
    this->zIndex = zIndex;
    this->space = space;
    isRect = true;
  }
};

struct RenderContext {
  RenderWindow *window;
  AssetManager *assetMan;
  std::vector<RenderCommand> renderQueue;
  RenderContext(RenderWindow *win, AssetManager *assets)
      : window(win), assetMan(assets),
        renderQueue(std::vector<RenderCommand>()) {}
};

void renderSystem(Registry &reg, RenderContext &ctx);
void sortRenderQueue(std::vector<RenderCommand> &renderQueue);
void drawRenderQueue(RenderContext &ctx);
void drawNineSlice(NineSlice &nineSlice, SDL_Color tint, RenderContext &ctx);
void collectRectRenderers(Registry &reg, RenderContext &ctx);
void collectProgressBars(Registry &reg, RenderContext &ctx);

} // namespace vl
