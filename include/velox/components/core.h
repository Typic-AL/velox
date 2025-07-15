#pragma once

#include "../Math.h"
#include "../util.h"
#include <SDL3/SDL.h>
#include <string>

namespace vl {

struct Position {
  Vec2 pos{};
};
struct Velocity {
  Vec2 vel{};
};
struct SpriteRenderer {
  SDL_Texture *tex = nullptr;
  int zIndex = 0;
  SDL_FRect src{};
  SDL_FRect dst{};
  bool isUi = false;
  bool useRenderScale = false;
};
struct TextRenderer {
  const char *fontPath;
  std::string text;
  Vec2 pos;
  SDL_Color color = vl::BLACK;
  int size;
  int zIndex;
  bool centered = false;
  bool pixelFont = false;
  bool useRenderScale = false;
};
struct RectRenderer {
  Vec2 pos;
  float width, height;
  SDL_Color color;
  float alpha = 255;
  int zIndex = 0;
};

} // namespace vl
