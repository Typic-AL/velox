#pragma once
#include <functional>
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

  template <typename Func, typename... Args>
  void registerSystem(Func &&func, Args &&...args) {
    auto boundFunc =
        std::bind(std::forward<Func>(func), this, std::forward<Args>(args)...);
    m_systems.push_back([f = std::move(boundFunc)]() { f(); });
  }

  void runSystems() {
      for(auto &sys : m_systems)
        sys();
  }

private:
  Entity nextEntityId = 0;

  std::vector<std::function<void()>> m_systems;

  template <typename T> std::unordered_map<Entity, T> &getComponentStorage() {
    static std::unordered_map<Entity, T> storage;
    return storage;
  }
};
} // namespace vl
