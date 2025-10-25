#pragma once

#include "velox/assetManager.h"
#include "velox/components/input.h"
#include "velox/registry.h"
#include "velox/scene.h"
#include "velox/systems/renderSystem.h"
#include "velox/renderWindow.h"

namespace vl {

class Engine {
private:
    RenderWindow m_window;
    AssetManager m_assetMan = AssetManager(&m_window);
    RenderContext renderCtx{&m_window, &m_assetMan};
    Registry m_reg;
    SceneManager m_sceneMan = SceneManager(m_reg);
public:
    Engine() {
        m_reg.emplaceResource<Input>();
        m_reg.setResource(m_sceneMan);
        m_reg.setResource(m_assetMan);
    }
   ~Engine() {}
  bool init(const char *title, int width, int height);
  void update();
  void render();

  void beginFrame();
  void endFrame();
  void setMaxFPS(int fps);
  void setPhysicsFPS(int fps);

  Registry &getReg() {return m_reg;}
  SceneManager &getSceneMan() {return m_sceneMan;}

  RenderWindow &getWindow() {return m_window;}
};

}
