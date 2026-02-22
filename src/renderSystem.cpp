#include "velox/systems/renderSystem.h"
#include "velox/renderWindow.h"

#include "velox/components/core.h"
#include "velox/registry.h"

namespace vl {

void sortRenderQueue(std::vector<RenderCommand> &renderQueue) {

  std::sort(renderQueue.begin(), renderQueue.end(),
            [](const RenderCommand &a, const RenderCommand &b) {
              if (a.isUi != b.isUi)
                return !a.isUi;

              if (a.zIndex != b.zIndex)
                return a.zIndex < b.zIndex;
              return a.tex < b.tex;
            });
}

void drawRenderQueue(RenderContext &ctx) {
  bool changedScale = false;

  SDL_Renderer *renderer = ctx.window->getRen();
  int refWidth = ctx.window->getReferenceWidth();
  int refHeight = ctx.window->getReferenceHeight();
  int width = ctx.window->getScreenWidth();
  int height = ctx.window->getScreenHeight();
  SDL_RendererLogicalPresentation presentationMode =
      ctx.window->getPresentationMode();

  for (auto &command : ctx.renderQueue) {
    if (command.useRenderScale) {
      if (changedScale)
        SDL_SetRenderLogicalPresentation(renderer, refWidth, refHeight,
                                         presentationMode);
      SDL_RenderTexture(renderer, command.tex, &command.srcRect,
                        &command.dstRect);
      changedScale = false;
    } else {
      SDL_SetRenderLogicalPresentation(renderer, width, height,
                                       SDL_LOGICAL_PRESENTATION_DISABLED);
      SDL_RenderTexture(renderer, command.tex, &command.srcRect,
                        &command.dstRect);
      changedScale = true;
    }
  }
}

void renderSystem(Registry &reg, RenderContext &ctx) {
  for (auto &e : reg.view<SpriteRenderer, Transform>()) {
    SpriteRenderer &sprite = reg.get<SpriteRenderer>(e);
    Transform &transform = reg.get<Transform>(e);
    SDL_Texture *tex = ctx.assetMan->idToTex(sprite.id);
    SDL_SetTextureScaleMode(tex, sprite.scaleMode);
    ctx.renderQueue.emplace_back(tex,
                                 SDL_FRect{transform.lPos.x, transform.lPos.y,
                                           sprite.width, sprite.height},
                                 sprite.src, sprite.zIndex, sprite.isUi,
                                 sprite.useRenderScale);
  }

  sortRenderQueue(ctx.renderQueue);
  drawRenderQueue(ctx);

  ctx.renderQueue.clear();
}

} // namespace vl
