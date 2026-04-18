#pragma once

#include "entity.h"
#include "sparseSet.h"

namespace vl {

struct ComponentPool {
  virtual ~ComponentPool() = default;

  virtual void remove(Entity e) = 0;
  virtual bool has(Entity e) = 0;
};

template <typename T> struct ComponentStorage : ComponentPool {
  SparseSet<T> data;

  void remove(Entity e) override { data.remove(e); }
  bool has(Entity e) override { return data.has(e); }
};

} // namespace vl
