#pragma once

#include "../components/core.h"
#include "velox/registry.h"
#include <functional>
#include <typeindex>
#include <unordered_map>

struct PositionUpdater {};

namespace vl {
class PositionSyncer {
private:
  static std::unordered_map<std::type_index, std::function<void(float, float)>>
      syncFuncs;

public:
  static void addSyncer(std::type_index t,
                        std::function<void(float, float)> func) {
    syncFuncs[t] = func;
  }

  void sync(Registry &reg) {
    for (auto &e : reg.view<PositionUpdater, Transform>()) {
      Transform &transform = reg.get<Transform>(e);
    }
  }
};
} // namespace vl
