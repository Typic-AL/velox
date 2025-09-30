#pragma once

#include "../util.h"
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>

#include "../resourceIDs.h"

namespace vl {

struct Transform {
  glm::vec2 pos{};
};

enum BodyType {
    STATIC,
    DYNAMIC,
    KINEMATIC
};

struct Rigidbody {
    glm::vec2 vel{};
    BodyType type = BodyType::DYNAMIC;
    float mass = 100.0f;
};
struct SpriteRenderer {
  TextureID id;
  int zIndex = 0;
  SDL_FRect src{};
  SDL_FRect dst{};
  bool isUi = false;
  bool useRenderScale = true;
  SDL_ScaleMode scaleMode = SDL_SCALEMODE_LINEAR;
};
struct TextRenderer {
  FontID id;
  std::string text;
  glm::vec2 pos;
  SDL_Color color = vl::BLACK;
  int size;
  int zIndex;
  bool centered = false;
  bool pixelFont = false;
  bool useRenderScale = false;
  bool isUi = true;
};
struct RectRenderer {
  glm::vec2 pos;
  float width, height;
  SDL_Color color;
  float alpha = 255;
  int zIndex = 0;
  bool useRenderScale = true;
  bool isUi = false;
};

struct PositionUpdater {
   std::vector<std::pair<float *, float *>> syncTargets;
};


} // namespace vl
