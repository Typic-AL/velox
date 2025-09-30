#include "velox/engine.h"
#include "velox/systems/renderSystem.h"
#include "velox/systems/core.h"
#include "velox/systems/collisionSystem.h"

#include "velox/time.h"

namespace vl {

bool Engine::init(const char *title, int width, int height) {
    if (m_window.init(title, width, height) == false)
        return false;
    if(m_assetMan.parseManifest() == false)
        return false;
    return true;
}

void Engine::update() {
    while(Physics::shouldUpdate()) {
        m_reg.runSystems();
        applyVelocity(m_reg);
        sweepAndPrune(m_reg);
        syncPositions(m_reg);
    }
   Physics::stepPhysics();


}

void Engine::render() {
    m_window.clear(WHITE);
    renderSystem(m_reg, renderCtx);
    m_window.present();
}
void Engine::beginFrame() {
    Time::beginFrame();
}

void Engine::endFrame() {
    Time::endFrame();
}

void Engine::setMaxFPS(int fps) {
    setTargetFPS(fps);
}

void Engine::setPhysicsFPS(int fps) {
    vl::setPhysicsFPS(fps);
}

}
