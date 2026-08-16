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
    auto it = sparse.find(e);
    if (it != sparse.end()) {
      dense[it->second] = component;
      return;
    }

    size_t index = dense.size();

    denseEntities.push_back(e);
    dense.push_back(component);

    sparse[e] = index;
  }

  T &get(Entity e) { return dense[sparse.at(e)]; }

  bool has(Entity e) { return sparse.find(e) != sparse.end(); }

  void remove(Entity e) {
    auto it = sparse.find(e);
    if (it == sparse.end())
      return;

    size_t idx = it->second;
    size_t last = dense.size() - 1;
    Entity moved = denseEntities[last];
    std::swap(dense[idx], dense[last]);
    std::swap(denseEntities[idx], denseEntities[last]);
    sparse[moved] = idx;
    dense.pop_back();
    denseEntities.pop_back();
    sparse.erase(e);
  }

  auto begin() { return dense.begin(); }
  auto end() { return dense.end(); }
};

} // namespace vl
