#pragma once

#include "registry.h"

namespace vl {

class System {
protected:
  Registry *m_reg;

public:
  System(Registry *reg) : m_reg(reg) {}
  virtual ~System() {};

  virtual void update(float dt);
};
} // namespace vl
