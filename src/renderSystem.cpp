#include "velox/systems/renderSystem.h"
#include "velox/renderWindow.h"

#include "velox/components/core.h"
#include "velox/components/ui.h"
#include "velox/registry.h"
#include "velox/assetManager.h"

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
  for (auto [tilemap, transform] : reg.view<TilemapRenderer, Transform>()) {
    if (tilemap.id.empty())
      continue;

    const TilemapData &data = ctx.assetMan->idToTilemap(tilemap.id);
    float originX = transform.lPos.x;
    float originY = transform.lPos.y;

    for (const auto &layer : data.layers) {
      if (!layer.visible)
        continue;

      if (!tilemap.layerFilter.empty()) {
        bool found = false;
        for (const auto &name : tilemap.layerFilter)
          if (name == layer.name) { found = true; break; }
        if (!found)
          continue;
      }

      float layerX = originX + layer.offsetX;
      float layerY = originY + layer.offsetY;

      for (int row = 0; row < layer.height; ++row) {
        for (int col = 0; col < layer.width; ++col) {
          int idx = row * layer.width + col;
          if (idx >= static_cast<int>(layer.data.size()))
            continue;

          int gid = layer.data[idx];
          if (gid == 0)
            continue;

          const TilemapTileset *ts = data.tilesetForGid(gid);
          if (!ts)
            continue;

          SDL_Texture *tex = ctx.assetMan->idToTex(ts->textureId);
          if (!tex)
            continue;

          SDL_FRect src = data.srcRectForGid(gid, *ts);
          SDL_FRect dst{
              layerX + col * static_cast<float>(data.tileWidth),
              layerY + row * static_cast<float>(data.tileHeight),
              static_cast<float>(data.tileWidth),
              static_cast<float>(data.tileHeight),
          };

          SDL_SetTextureAlphaMod(tex, static_cast<Uint8>(layer.opacity * 255.0f));
          ctx.renderQueue.emplace_back(tex, dst, src, tilemap.zIndex, false,
                                       tilemap.useRenderScale);
        }
      }
    }
  }

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
