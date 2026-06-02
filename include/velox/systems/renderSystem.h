#pragma once

#include "velox/assetManager.h"
#include "velox/components/ui/ui.h"
#include <vector>

namespace vl {

class RenderWindow;
class Registry;

struct RenderCommand {
  SDL_Texture *tex = nullptr;
  SDL_FRect dstRect;
  SDL_FRect srcRect;
  SDL_Color color = {255, 255, 255, 255};
  int zIndex = 0;
  bool useRenderScale = true;
  bool isUi = false;
  bool isRect = false;

  RenderCommand(SDL_Texture *tex, SDL_FRect dst, SDL_FRect src, int zIndex,
                bool isUi, bool useRenderScale) {
    this->tex = tex;
    dstRect = dst;
    srcRect = src;
    this->zIndex = zIndex;
    this->isUi = isUi;
    this->useRenderScale = useRenderScale;
  }

  RenderCommand(SDL_FRect dst, SDL_Color color, int zIndex, bool isUi) {
    dstRect = dst;
    this->color = color;
    this->zIndex = zIndex;
    this->isUi = isUi;
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
void drawNineSlice(NineSlice &nineSlice, RenderContext &ctx);
void collectRectRenderers(Registry &reg, RenderContext &ctx);
void collectProgressBars(Registry &reg, RenderContext &ctx);

} // namespace vl
