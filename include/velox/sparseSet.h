#pragma once

#include <unordered_map>
#include <vector>

#include "entity.h"

namespace vl {

template <typename T> struct SparseSet {
  std::vector<T> dense;
  std::vector<Entity> denseEntities;
  std::unordered_map<Entity, size_t> sparse;

  void insert(Entity e, T component) {
    size_t index = dense.size();

    denseEntities.push_back(e);
    dense.push_back(component);

    sparse[e] = index;
  }

  T &get(Entity e) { return dense[sparse[e]]; }

  bool has(Entity e) { return sparse.find(e) != sparse.end(); }

  void remove(Entity e) {
    std::swap(dense[sparse[e]], dense[dense.size() - 1]);
    std::swap(denseEntities[sparse[e]],
              denseEntities[denseEntities.size() - 1]);

    dense.pop_back();
    denseEntities.pop_back();
  }

  auto begin() { return dense.begin(); }
  auto end() { return dense.end(); }
};

} // namespace vl
