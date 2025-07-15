#pragma once

#include "registry.h"

namespace vl {

class Scene {
public:
  virtual void load(Registry &reg) = 0;
  virtual void update(float dt, Registry &reg) = 0;
  // virtual void render(Registry& reg, RenderWindow& window) = 0;
  virtual void unload(Registry &reg) {} // Optional
  virtual ~Scene() = default;
};

} // namespace vl
