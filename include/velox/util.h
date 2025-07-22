#pragma once

#include <SDL3/SDL.h>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

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

inline std::string floatToStr(float value) {
  std::ostringstream oss;
  oss << std::fixed << std::setprecision(6) << value; // Up to 6 decimals
  std::string str = oss.str();

  // Trim trailing zeros
  str.erase(str.find_last_not_of('0') + 1, std::string::npos);

  // If it ends with '.', remove that too
  if (!str.empty() && str.back() == '.')
    str.pop_back();

  return str;
}
} // namespace vl
