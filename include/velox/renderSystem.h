#pragma once

#include "system.h"

namespace vl {

class RenderWindow;

class RenderSystem : public System {
private:
  RenderWindow *m_window;

public:
  RenderSystem(Registry *reg, RenderWindow *window)
      : m_window(window), System(reg) {}

  void update(float dt) override;
};
} // namespace vl
