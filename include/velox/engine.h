#pragma once

#include "velox/assetManager.h"
#include "velox/registry.h"
#include "velox/renderSystem.h"
#include "velox/renderWindow.h"

namespace vl {

class Engine {
private:
    RenderWindow m_window;
    AssetManager m_assetMan = AssetManager(&m_window);
    RenderContext renderCtx{&m_window, &m_assetMan};
    Registry *m_currentReg;
public:
    Engine() {}
   ~Engine() {}
  bool init(const char *title, int width, int height);
  void update();
  void render();

  void beginFrame();
  void endFrame();
  void setMaxFPS(int fps);
  void setPhysicsFPS(int fps);

  void setCurrentRegistry(Registry *reg) {m_currentReg = reg;}

  RenderWindow &getWindow() {return m_window;}
};

}
