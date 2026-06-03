#pragma once

#include "../util.h"
#include <SDL3/SDL.h>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

#include "../resourceIDs.h"
#include "../space.h"

namespace vl {

struct Transform {
  glm::vec2 pos{};
  glm::vec2 prevPos{};
  glm::vec2 lPos{};

  Transform() : pos(0, 0), prevPos(0, 0), lPos(0, 0) {}
  Transform(glm::vec2 pos) : pos(pos), prevPos(pos), lPos(pos) {}
};

enum BodyType { STATIC, DYNAMIC, KINEMATIC };

struct Rigidbody {
  glm::vec2 vel{};
  BodyType type = BodyType::DYNAMIC;
  float mass = 100.0f;
};
struct SpriteRenderer {
  TextureID id;
  int zIndex = 0;
  SDL_FRect src{};
  float width;
  float height;
  Space space = Space::WORLD;
  bool useRenderScale = true;
  SDL_ScaleMode scaleMode = SDL_SCALEMODE_LINEAR;
};

struct TextRenderer {
  FontID id;
  std::string text;
  SDL_Color color = vl::BLACK;
  int size;
  int zIndex;
  bool centered = false;
  bool pixelFont = false;
  bool useRenderScale = false;
  Space space = Space::SCREEN;
};
struct RectRenderer {
  float width, height;
  SDL_Color color = {255, 255, 255, 255};
  int zIndex = 0;
  bool useRenderScale = true;
  Space space = Space::WORLD;
};

} // namespace vl
