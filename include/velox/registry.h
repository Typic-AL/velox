#pragma once
#include <functional>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <any>

#include "entity.h"
#include "components/input.h"


namespace vl {

class Registry {
public:

  Entity create() { return nextEntityId++; }

  template <typename T>
  void addComponent(Entity e, T component) {
    getComponentStorage<T>()[e] = component;
  }

  template <typename T>
  T &get(Entity e) {
    return getComponentStorage<T>().at(e);
  }


  template<typename... Components>
  std::vector<Entity> view() {
      // Compile-time check
      static_assert(sizeof...(Components) > 0, "Must provide at least one component type");

      // Start with the smallest component set as base (for optimization)
      auto &firstStorage = getComponentStorage<std::tuple_element_t<0, std::tuple<Components...>>>();

      std::vector<Entity> result;

      for (auto &[entity, _] : firstStorage) {
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
    static_assert(std::is_invocable_r_v<void, Func, Registry&, Args...>,
                "func must be callable with (Registry&, [extra args...]) and return void");

    auto boundFunc = std::bind(std::forward<Func>(func), std::ref(*this), std::forward<Args>(args)...);

    m_systems.push_back([f = std::move(boundFunc)]() { f(); });
  }

  void runSystems() {
      for(auto &sys : m_systems)
        sys();
  }

  void clear() {
      nextEntityId = 0;
      m_systems.clear();
  }

  template<typename T, typename... Args>
  T& emplaceResource(Args&&... args) {
      auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
      T& ref = *ptr;
      m_resources[std::type_index(typeid(T))] = std::move(ptr);
      return ref;
  }

  // Add an existing resource (non-owning)
  template<typename T>
  void setResource(T& resource) {
      m_resources[std::type_index(typeid(T))] = std::shared_ptr<void>(&resource, [](void*) {});
  }

  // Get a mutable reference
  template<typename T>
  T& getResource() {
      auto it = m_resources.find(std::type_index(typeid(T)));
      if (it == m_resources.end()) {
          throw std::runtime_error("Resource not found");
      }
      return *static_cast<T*>(it->second.get());
  }

  // Get a const reference
  template<typename T>
  const T& getResource() const {
      auto it = m_resources.find(std::type_index(typeid(T)));
      if (it == m_resources.end()) {
          throw std::runtime_error("Resource not found");
      }
      return *static_cast<const T*>(it->second.get());
  }

private:
  Entity nextEntityId = 0;

  std::vector<std::function<void()>> m_systems;
  std::unordered_map<std::type_index, std::shared_ptr<void>> m_resources;


  std::unordered_map<std::type_index, std::any> componentStorages;

  //template <typename T> std::unordered_map<Entity, T> &getComponentStorage() {
    //static std::unordered_map<Entity, T> storage;
    //return storage;
    //}
  template <typename T>
  std::unordered_map<Entity, T>& getComponentStorage() {
      std::type_index type = std::type_index(typeid(T));

      if (!componentStorages.contains(type)) {
          componentStorages[type] = std::unordered_map<Entity, T>();
      }

      return std::any_cast<std::unordered_map<Entity, T>&>(componentStorages[type]);
  }
};
} // namespace vl
