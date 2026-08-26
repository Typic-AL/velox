#pragma once

#include <SDL3/SDL.h>
#include <cstddef>
#include <functional>
#include <string>
#include <utility>

namespace vl {

inline SDL_Color WHITE = SDL_Color{255, 255, 255, 255};
inline SDL_Color BLACK = SDL_Color{0, 0, 0, 255};
inline SDL_Color RED = SDL_Color{255, 0, 0, 255};
inline SDL_Color GREEN = SDL_Color{0, 255, 0, 255};
inline SDL_Color BLUE = SDL_Color{0, 0, 255, 255};

template <typename T1, typename T2> struct PairHash {
  std::size_t operator()(const std::pair<T1, T2> &p) const {
    std::size_t h1 = std::hash<T1>{}(p.first);
    std::size_t h2 = std::hash<T2>{}(p.second);
    return h1 ^ (h2 << 1); // or a better mixing function
  }
};

std::string floatToStr(float value);

} // namespace vl
