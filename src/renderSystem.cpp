#include <algorithm>
#include "velox/systems/renderSystem.h"
#include "velox/renderWindow.h"

#include "velox/assetManager.h"
#include "velox/camera.h"
#include "velox/components/core.h"
#include "velox/components/ui/ui.h"
#include "velox/registry.h"

namespace vl {

void sortRenderQueue(std::vector<RenderCommand> &renderQueue) {
  std::stable_sort(renderQueue.begin(), renderQueue.end(),
                   [](const RenderCommand &a, const RenderCommand &b) {
                     if (a.space != b.space)
                       return a.space == Space::WORLD;

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
      changedScale = false;
    } else {
      SDL_SetRenderLogicalPresentation(renderer, width, height,
                                       SDL_LOGICAL_PRESENTATION_DISABLED);
      changedScale = true;
    }

    if (command.isRect) {
      SDL_SetRenderDrawColor(renderer, command.color.r, command.color.g,
                             command.color.b, command.color.a);
      SDL_RenderFillRect(renderer, &command.dstRect);
    } else {
      SDL_RenderTexture(renderer, command.tex, &command.srcRect,
                        &command.dstRect);
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
                                 nineSlice.space, nineSlice.useRenderScale);
  }
}

struct CameraOffsets {
  glm::vec2 scaled{0, 0};
  glm::vec2 raw{0, 0};

  glm::vec2 pick(Space space, bool useRenderScale) const {
    if (space == Space::SCREEN)
      return {0, 0};
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
  off.scaled =
      cam->pos + cam->followOffset - glm::vec2(refW * 0.5f, refH * 0.5f);
  off.raw =
      (cam->pos + cam->followOffset) * glm::vec2(scrW / refW, scrH / refH) -
      glm::vec2(scrW * 0.5f, scrH * 0.5f);
  return off;
}

void collectTilemaps(Registry &reg, RenderContext &ctx,
                     const CameraOffsets &off) {
  for (auto [tilemap, transform] : reg.view<TilemapRenderer, Transform>()) {
    if (tilemap.id.empty())
      continue;

    const TilemapData &data = ctx.assetMan->idToTilemap(tilemap.id);
    glm::vec2 tmOff = off.pick(Space::WORLD, tilemap.useRenderScale);
    float originX = transform.lPos.x - tmOff.x;
    float originY = transform.lPos.y - tmOff.y;

    for (const auto &layer : data.layers) {
      if (!layer.visible)
        continue;

      if (!tilemap.layerFilter.empty()) {
        bool found = false;
        for (const auto &name : tilemap.layerFilter)
          if (name == layer.name) {
            found = true;
            break;
          }
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

          int gid = layer.data[idx] & 0x1FFFFFFF;
          if (gid == 0)
            continue;

          const TilemapTileset *ts = data.tilesetForGid(gid);
          if (!ts)
            continue;

          SDL_Texture *tex = ctx.assetMan->idToTex(ts->textureId);
          if (!tex)
            continue;

          SDL_SetTextureScaleMode(tex, tilemap.scaleMode);

          SDL_FRect src = data.srcRectForGid(gid, *ts);
          SDL_FRect dst{
              layerX + col * static_cast<float>(data.tileWidth),
              layerY + row * static_cast<float>(data.tileHeight),
              static_cast<float>(data.tileWidth),
              static_cast<float>(data.tileHeight),
          };

          SDL_SetTextureAlphaMod(tex,
                                 static_cast<Uint8>(layer.opacity * 255.0f));
          ctx.renderQueue.emplace_back(tex, dst, src, tilemap.zIndex - 1000,
                                       Space::WORLD, tilemap.useRenderScale);
        }
      }
    }
  }
}

void collectSprites(Registry &reg, RenderContext &ctx,
                    const CameraOffsets &off) {
  for (auto [sprite, transform] : reg.view<SpriteRenderer, Transform>()) {
    SDL_Texture *tex = ctx.assetMan->idToTex(sprite.id);
    SDL_SetTextureScaleMode(tex, sprite.scaleMode);
    glm::vec2 o = off.pick(sprite.space, sprite.useRenderScale);
    ctx.renderQueue.emplace_back(
        tex,
        SDL_FRect{transform.lPos.x - o.x, transform.lPos.y - o.y, sprite.width,
                  sprite.height},
        sprite.src, sprite.zIndex, sprite.space, sprite.useRenderScale);
  }
}

void collectText(Registry &reg, RenderContext &ctx, const CameraOffsets &off) {
  for (auto [text, transform] : reg.view<TextRenderer, Transform>()) {
    SDL_Texture *tex =
        ctx.assetMan->getTextTex(text.text, text.id, text.size, text.color);
    if (!tex)
      continue;
    float w, h;
    SDL_GetTextureSize(tex, &w, &h);
    SDL_SetTextureScaleMode(tex, text.pixelFont ? SDL_SCALEMODE_NEAREST
                                                : SDL_SCALEMODE_LINEAR);
    glm::vec2 o = off.pick(text.space, text.useRenderScale);
    float x = transform.lPos.x - o.x;
    float y = transform.lPos.y - o.y;
    if (text.centered) {
      x -= w / 2.0f;
      y -= h / 2.0f;
    }
    ctx.renderQueue.emplace_back(tex, SDL_FRect{x, y, w, h},
                                 SDL_FRect{0, 0, w, h}, text.zIndex, text.space,
                                 text.useRenderScale);
  }
}

void collectNineSlices(Registry &reg, RenderContext &ctx) {
  for (auto [nineSlice] : reg.view<NineSlice>())
    drawNineSlice(nineSlice, ctx);
}

void collectRectRenderers(Registry &reg, RenderContext &ctx) {
  CameraOffsets off = computeCameraOffsets(reg, ctx);
  for (auto [rect, transform] : reg.view<RectRenderer, Transform>()) {
    glm::vec2 o = off.pick(rect.space, rect.useRenderScale);
    float x = transform.lPos.x - o.x;
    float y = transform.lPos.y - o.y;
    SDL_Color col = {rect.color.r, rect.color.g, rect.color.b,
                     static_cast<Uint8>(rect.alpha)};
    ctx.renderQueue.emplace_back(SDL_FRect{x, y, rect.width, rect.height}, col,
                                 rect.zIndex, rect.space);
  }
}

void collectProgressBars(Registry &reg, RenderContext &ctx) {
  CameraOffsets off = computeCameraOffsets(reg, ctx);
  for (auto [bar, transform] : reg.view<ProgressBar, Transform>()) {
    glm::vec2 o = off.pick(bar.space, true);
    float x = transform.lPos.x - o.x;
    float y = transform.lPos.y - o.y;

    ctx.renderQueue.emplace_back(SDL_FRect{x, y, bar.w, bar.h}, bar.bgColor,
                                 bar.zIndex, bar.space);
    ctx.renderQueue.emplace_back(
        SDL_FRect{x, y, bar.displayValue * bar.w, bar.h}, bar.fillColor,
        bar.zIndex + 1, bar.space);
  }
}

void renderSystem(Registry &reg, RenderContext &ctx) {
  CameraOffsets off = computeCameraOffsets(reg, ctx);

  collectTilemaps(reg, ctx, off);
  collectSprites(reg, ctx, off);
  collectText(reg, ctx, off);
  collectNineSlices(reg, ctx);
  collectRectRenderers(reg, ctx);
  collectProgressBars(reg, ctx);

  sortRenderQueue(ctx.renderQueue);
  drawRenderQueue(ctx);
  ctx.renderQueue.clear();
}

} // namespace vl
