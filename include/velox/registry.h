#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "componentStorage.h"
#include "entity.h"
#include "velox/sparseSet.h"

namespace vl {

class Registry;

class Registry {
public:
  Entity create() { return nextEntityId++; }

  template <typename... Args> Entity createWith(Args &&...components) {
    Entity e = create();
    (addComponent(e, components), ...);
    return e;
  }

  template <typename T> void addComponent(Entity e, T component) {
    getComponentStorage<T>().insert(e, component);
  }

  template <typename T> T &get(Entity e) {
    return getComponentStorage<T>().get(e);
  }

  template <typename... Components> struct ViewProxy {
    Registry &reg;
    std::vector<Entity> entities;

    template <typename... Tags> ViewProxy with() && {
      std::erase_if(entities,
                    [&](Entity e) { return (!reg.has<Tags>(e) || ...); });

      return std::move(*this);
    }

    template <bool WithEntity> struct Iterator {
      Registry &reg;
      std::vector<Entity>::iterator it;

      auto operator*() {
        if constexpr (WithEntity)
          return std::tuple<Components &..., Entity>{
              reg.get<Components>(*it)..., *it};
        else
          return std::tuple<Components &...>{reg.get<Components>(*it)...};
      }

      Iterator &operator++() {
        ++it;
        return *this;
      }
      bool operator!=(const Iterator &o) { return it != o.it; }
    };

    Iterator<false> begin() { return {reg, entities.begin()}; }
    Iterator<false> end() { return {reg, entities.end()}; }

    struct EntityView {
      Registry &reg;
      std::vector<Entity> entities;

      Iterator<true> begin() { return {reg, entities.begin()}; }
      Iterator<true> end() { return {reg, entities.end()}; }
    };

    EntityView withEntity() { return {reg, std::move(entities)}; }
  };

  template <typename... Components> ViewProxy<Components...> view() {
    // Compile-time check
    static_assert(sizeof...(Components) > 0,
                  "Must provide at least one component type");

    auto &firstStorage = getComponentStorage<
        std::tuple_element_t<0, std::tuple<Components...>>>();
    std::vector<Entity> result;

    for (auto entity : firstStorage.denseEntities) {
      if ((has<Components>(entity) && ...)) {
        result.push_back(entity);
      }
    }

    return ViewProxy<Components...>{*this, std::move(result)};
  }

  template <typename T> bool has(Entity e) {
    return getComponentStorage<T>().has(e);
  }

  template <typename Func, typename... Args>
  void registerSystem(Func &&func, Args &&...args) {
    static_assert(std::is_invocable_r_v<void, Func, Registry &, Args...>,
                  "func must be callable with (Registry&, [extra args...]) and "
                  "return void");

    auto boundFunc = std::bind(std::forward<Func>(func), std::ref(*this),
                               std::forward<Args>(args)...);

    m_systems.push_back([f = std::move(boundFunc)]() { f(); });
  }

  void runSystems() {
    for (auto &sys : m_systems)
      sys();
  }

  void clear() {
    nextEntityId = 0;
    m_systems.clear();
    componentStorages.clear();
  }

  template <typename T, typename... Args> T &emplaceResource(Args &&...args) {
    auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
    T &ref = *ptr;
    m_resources[std::type_index(typeid(T))] = std::move(ptr);
    return ref;
  }

  // Add an existing resource (non-owning)
  template <typename T> void setResource(T &resource) {
    m_resources[std::type_index(typeid(T))] =
        std::shared_ptr<void>(&resource, [](void *) {});
  }

  // Get a mutable reference
  template <typename T> T &getResource() {
    auto it = m_resources.find(std::type_index(typeid(T)));
    if (it == m_resources.end()) {
      throw std::runtime_error(std::string("Resource not found: ") +
                               typeid(T).name());
    }
    return *static_cast<T *>(it->second.get());
  }

  template <typename T> T *tryGetResource() {
    auto it = m_resources.find(std::type_index(typeid(T)));
    if (it == m_resources.end())
      return nullptr;
    return static_cast<T *>(it->second.get());
  }

  // Get a const reference
  template <typename T> const T &getResource() const {
    auto it = m_resources.find(std::type_index(typeid(T)));
    if (it == m_resources.end()) {
      throw std::runtime_error("Resource not found");
    }
    return *static_cast<const T *>(it->second.get());
  }

private:
  Entity nextEntityId = 0;

  std::vector<std::function<void()>> m_systems;
  std::unordered_map<std::type_index, std::shared_ptr<void>> m_resources;

  std::unordered_map<std::type_index, std::unique_ptr<ComponentPool>>
      componentStorages;

  template <typename T> SparseSet<T> &getComponentStorage() {
    std::unique_ptr<ComponentPool> &ptr =
        componentStorages[std::type_index(typeid(T))];

    if (!ptr)
      ptr = std::make_unique<ComponentStorage<T>>();
    return static_cast<ComponentStorage<T> *>(ptr.get())->data;
  }
};
} // namespace vl
