#pragma once

#include "registry.h"
#include <string>

namespace vl {

class Application {
public:
  virtual ~Application() = default;

  virtual bool Init() = 0;

  virtual void LoadScene(const std::string &sceneName,
                         Registry &targetRegistry) = 0;

  virtual void PreUpdate(float deltaTime) {}

  virtual void PostUpdate(float deltaTime) {}

  virtual void Shutdown() = 0;

  virtual std::string GetInitialSceneName() const = 0;
};
} // namespace vl
