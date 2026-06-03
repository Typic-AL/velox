#pragma once

#include <SDL3/SDL.h>

namespace vl {

struct Tintable {
  SDL_Color tint = {255, 255, 255, 255};
};

} // namespace vl
