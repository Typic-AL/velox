#include "velox/systems/renderSystem.h"
#include "velox/renderWindow.h"

#include "velox/components/core.h"
#include "velox/components/ui.h"
#include "velox/registry.h"

namespace vl {

void sortRenderQueue(std::vector<RenderCommand> &renderQueue) {
  std::stable_sort(renderQueue.begin(), renderQueue.end(),
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

void drawNineSlice(NineSlice &nineSlice, RenderContext &ctx) {
  SDL_Texture *tex = ctx.assetMan->idToTex(nineSlice.id);
  if (!tex)
    return;

  SDL_SetTextureScaleMode(tex, nineSlice.scaleMode);

  float texW, texH;
  SDL_GetTextureSize(tex, &texW, &texH);

  float cs = nineSlice.cornerSize;
  float midSrcW = texW - cs * 2.0f;
  float midSrcH = texH - cs * 2.0f;

  float dx = nineSlice.rect.x;
  float dy = nineSlice.rect.y;
  float dw = nineSlice.rect.w;
  float dh = nineSlice.rect.h;
  float midDstW = dw - cs * 2.0f;
  float midDstH = dh - cs * 2.0f;

  SDL_FRect src[9] = {
      {0, 0, cs, cs},
      {cs, 0, midSrcW, cs},
      {texW - cs, 0, cs, cs},
      {0, cs, cs, midSrcH},
      {cs, cs, midSrcW, midSrcH},
      {texW - cs, cs, cs, midSrcH},
      {0, texH - cs, cs, cs},
      {cs, texH - cs, midSrcW, cs},
      {texW - cs, texH - cs, cs, cs},
  };

  SDL_FRect dst[9] = {
      {dx, dy, cs, cs},
      {dx + cs, dy, midDstW, cs},
      {dx + dw - cs, dy, cs, cs},
      {dx, dy + cs, cs, midDstH},
      {dx + cs, dy + cs, midDstW, midDstH},
      {dx + dw - cs, dy + cs, cs, midDstH},
      {dx, dy + dh - cs, cs, cs},
      {dx + cs, dy + dh - cs, midDstW, cs},
      {dx + dw - cs, dy + dh - cs, cs, cs},
  };

  for (int i = 0; i < 9; i++) {
    ctx.renderQueue.emplace_back(tex, dst[i], src[i], nineSlice.zIndex,
                                 nineSlice.isUi, nineSlice.useRenderScale);
  }
}

void renderSystem(Registry &reg, RenderContext &ctx) {
  for (auto [sprite, transform] : reg.view<SpriteRenderer, Transform>()) {
    SDL_Texture *tex = ctx.assetMan->idToTex(sprite.id);
    SDL_SetTextureScaleMode(tex, sprite.scaleMode);
    ctx.renderQueue.emplace_back(tex,
                                 SDL_FRect{transform.lPos.x, transform.lPos.y,
                                           sprite.width, sprite.height},
                                 sprite.src, sprite.zIndex, sprite.isUi,
                                 sprite.useRenderScale);
  }

  for (auto [text, transform] : reg.view<TextRenderer, Transform>()) {
    SDL_Texture *tex = ctx.assetMan->getTextTex(text.text, text.id, text.size, text.color);
    if (!tex)
      continue;
    float w, h;
    SDL_GetTextureSize(tex, &w, &h);
    SDL_SetTextureScaleMode(tex, text.pixelFont ? SDL_SCALEMODE_NEAREST
                                                : SDL_SCALEMODE_LINEAR);
    float x = transform.lPos.x;
    float y = transform.lPos.y;
    if (text.centered) {
      x -= w / 2.0f;
      y -= h / 2.0f;
    }
    ctx.renderQueue.emplace_back(tex, SDL_FRect{x, y, w, h},
                                 SDL_FRect{0, 0, w, h}, text.zIndex, text.isUi,
                                 text.useRenderScale);
  }

  for (auto [nineSlice] : reg.view<NineSlice>()) {
    drawNineSlice(nineSlice, ctx);
  }

  sortRenderQueue(ctx.renderQueue);
  drawRenderQueue(ctx);

  ctx.renderQueue.clear();
}

} // namespace vl
