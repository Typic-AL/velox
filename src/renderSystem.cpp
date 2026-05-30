#include "velox/systems/renderSystem.h"
#include "velox/renderWindow.h"

#include "velox/camera.h"
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

struct CameraOffsets {
  glm::vec2 scaled{0, 0};  // for useRenderScale=true (reference resolution space)
  glm::vec2 raw{0, 0};     // for useRenderScale=false (screen pixel space)

  glm::vec2 pick(bool isUi, bool useRenderScale) const {
    if (isUi) return {0, 0};
    return useRenderScale ? scaled : raw;
  }
};

CameraOffsets computeCameraOffsets(Registry &reg, RenderContext &ctx) {
  Camera *cam = reg.tryGetResource<Camera>();
  if (!cam)
    return {};

  float refW = static_cast<float>(ctx.window->getReferenceWidth());
  float refH = static_cast<float>(ctx.window->getReferenceHeight());
  float scrW = static_cast<float>(ctx.window->getScreenWidth());
  float scrH = static_cast<float>(ctx.window->getScreenHeight());

  CameraOffsets off;
  off.scaled = cam->pos - glm::vec2(refW * 0.5f, refH * 0.5f);
  off.raw    = cam->pos * glm::vec2(scrW / refW, scrH / refH) - glm::vec2(scrW * 0.5f, scrH * 0.5f);
  return off;
}

void collectTilemaps(Registry &reg, RenderContext &ctx, const CameraOffsets &off) {
  for (auto [tilemap, transform] : reg.view<TilemapRenderer, Transform>()) {
    if (tilemap.id.empty())
      continue;

    const TilemapData &data = ctx.assetMan->idToTilemap(tilemap.id);
    glm::vec2 tmOff = off.pick(false, tilemap.useRenderScale);
    float originX = transform.lPos.x - tmOff.x;
    float originY = transform.lPos.y - tmOff.y;

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
}

void collectSprites(Registry &reg, RenderContext &ctx, const CameraOffsets &off) {
  for (auto [sprite, transform] : reg.view<SpriteRenderer, Transform>()) {
    SDL_Texture *tex = ctx.assetMan->idToTex(sprite.id);
    SDL_SetTextureScaleMode(tex, sprite.scaleMode);
    glm::vec2 o = off.pick(sprite.isUi, sprite.useRenderScale);
    ctx.renderQueue.emplace_back(tex,
                                 SDL_FRect{transform.lPos.x - o.x,
                                           transform.lPos.y - o.y,
                                           sprite.width, sprite.height},
                                 sprite.src, sprite.zIndex, sprite.isUi,
                                 sprite.useRenderScale);
  }
}

void collectText(Registry &reg, RenderContext &ctx, const CameraOffsets &off) {
  for (auto [text, transform] : reg.view<TextRenderer, Transform>()) {
    SDL_Texture *tex = ctx.assetMan->getTextTex(text.text, text.id, text.size, text.color);
    if (!tex)
      continue;
    float w, h;
    SDL_GetTextureSize(tex, &w, &h);
    SDL_SetTextureScaleMode(tex, text.pixelFont ? SDL_SCALEMODE_NEAREST : SDL_SCALEMODE_LINEAR);
    glm::vec2 o = off.pick(text.isUi, text.useRenderScale);
    float x = transform.lPos.x - o.x;
    float y = transform.lPos.y - o.y;
    if (text.centered) {
      x -= w / 2.0f;
      y -= h / 2.0f;
    }
    ctx.renderQueue.emplace_back(tex, SDL_FRect{x, y, w, h},
                                 SDL_FRect{0, 0, w, h}, text.zIndex, text.isUi,
                                 text.useRenderScale);
  }
}

void collectNineSlices(Registry &reg, RenderContext &ctx) {
  for (auto [nineSlice] : reg.view<NineSlice>())
    drawNineSlice(nineSlice, ctx);
}

void renderSystem(Registry &reg, RenderContext &ctx) {
  CameraOffsets off = computeCameraOffsets(reg, ctx);

  collectTilemaps(reg, ctx, off);
  collectSprites(reg, ctx, off);
  collectText(reg, ctx, off);
  collectNineSlices(reg, ctx);

  sortRenderQueue(ctx.renderQueue);
  drawRenderQueue(ctx);
  ctx.renderQueue.clear();
}

} // namespace vl
