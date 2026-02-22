#pragma once

#include "velox/assetManager.h"
#include "velox/input.h"
#include "velox/registry.h"
#include "velox/renderWindow.h"
#include "velox/scene.h"
#include "velox/systems/renderSystem.h"

namespace vl {

class Engine {
private:
  RenderWindow m_window;
  AssetManager m_assetMan = AssetManager(&m_window);
  RenderContext renderCtx{&m_window, &m_assetMan};
  Registry m_reg;
  SceneManager m_sceneMan = SceneManager(m_reg);
  Input m_input = Input(m_window);

public:
  Engine() {
    m_reg.setResource(m_input);
    m_reg.setResource(m_sceneMan);
    m_reg.setResource(m_assetMan);
    m_reg.setResource(m_window);
  }
  ~Engine() {}
  bool init(const char *title, int width, int height);
  void update();
  void render();

  void beginFrame();
  void endFrame();
  void setMaxFPS(int fps);
  void setPhysicsFPS(int fps);

  Registry &getReg() { return m_reg; }
  SceneManager &getSceneMan() { return m_sceneMan; }

  RenderWindow &getWindow() { return m_window; }
};

} // namespace vl
