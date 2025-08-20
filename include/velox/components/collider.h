#pragma once

#include <SDL3/SDL.h>

namespace vl {

struct Collider {
  SDL_FRect rect;
  Collider(SDL_FRect rect) : rect(rect) {}
};
}
