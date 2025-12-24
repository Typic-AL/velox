#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

#include "../registry.h"
#include <functional>
#include <utility>

namespace vl {

using ButtonCallback = std::function<void(Registry &)>;

enum class AnchorX { LEFT, CENTER, RIGHT };
enum class AnchorY { TOP, CENTER, BOTTOM };

struct UITransform {
  std::pair<AnchorX, AnchorY> anchor;
  glm::vec2 offset;
};

struct UIButton {
  SDL_FRect rect;
  ButtonCallback callback;
};

} // namespace vl
