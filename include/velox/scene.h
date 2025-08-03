#pragma once

#include "registry.h"

namespace vl {

class Scene {
protected:
  Registry m_reg;

public:
  virtual ~Scene() = default;
  virtual void load(Registry &reg) = 0;
  virtual void update(float dt) = 0;
  // virtual void render(Registry& reg, RenderWindow& window) = 0;
  virtual void unload(Registry &reg) {} // Optional
};
} // namespace vl
