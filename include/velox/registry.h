#pragma once
#include <any>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "entity.h"

namespace vl {

class Registry {
public:
  Entity create() { return nextEntityId++; }

  template <typename T> void addComponent(Entity e, T component) {
    getComponentStorage<T>()[e] = component;
  }

  template <typename T> T &get(Entity e) {
    return getComponentStorage<T>().at(e);
  }

  template <typename... Components> std::vector<Entity> view() {
    std::vector<Entity> result;

    auto &first = getComponentStorage<Components...>();

    for (auto &[entity, _] : first) {
      if ((hasComponent<Components>(entity) && ...)) {
        result.push_back(entity);
      }
    }

    return result;
  }

  template <typename T> bool hasComponent(Entity e) {
    return getComponentStorage<T>().count(e);
  }

private:
  Entity nextEntityId = 0;

  template <typename T> std::unordered_map<Entity, T> &getComponentStorage() {
    static std::unordered_map<Entity, T> storage;
    return storage;
  }
};
} // namespace vl
