#pragma once

#include "velox/assetManager.h"
#include <vector>

namespace vl {

class RenderWindow;
class Registry;

struct RenderCommand {
  SDL_Texture *tex;
  SDL_FRect dstRect;
  SDL_FRect srcRect;
  int zIndex = 0;
  bool useRenderScale = true;
  bool isUi = false;
  RenderCommand(SDL_Texture *tex, SDL_FRect dst, SDL_FRect src, int zIndex,
                bool isUi, bool useRenderScale) {
    this->tex = tex;
    dstRect = dst;
    srcRect = src;
    this->zIndex = zIndex;
    this->isUi = isUi;
    this->useRenderScale = useRenderScale;
  }
};

struct RenderContext {
  RenderWindow *window;
  AssetManager *assetMan;
  std::vector<RenderCommand> renderQueue;
  RenderContext(RenderWindow *win, AssetManager * assets) : window(win), assetMan(assets), renderQueue(std::vector<RenderCommand>()) {}
};

void renderSystem(Registry *reg, RenderContext &ctx);
void sortRenderQueue(std::vector<RenderCommand> &renderQueue);
void drawRenderQueue(RenderContext &ctx);

} // namespace vl
