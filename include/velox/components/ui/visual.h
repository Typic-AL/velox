#pragma once

#include "velox/registry.h"
#include "velox/resourceIDs.h"
#include <SDL3/SDL.h>

namespace vl {

struct NineSlice {
  TextureID id;
  SDL_FRect rect;
  float cornerSize = 8;
  int zIndex = 0;
  SDL_ScaleMode scaleMode = SDL_SCALEMODE_LINEAR;
  bool useRenderScale = true;
  bool isUi = true;
};

struct ProgressBar {
  float w, h;
  float value = 1.0f;
  float displayValue = 1.0f;
  float speed = 5.0f;
  SDL_Color fillColor = {255, 255, 255, 255};
  SDL_Color bgColor = {50, 50, 50, 255};
  int zIndex = 0;
  bool isUi = true;
};

void updateProgressBars(Registry &reg);

} // namespace vl
