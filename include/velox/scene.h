#pragma once

#include "registry.h"
#include <cassert>
#include <functional>
#include <unordered_map>

namespace vl {

class SceneManager {
private:
    std::unordered_map<std::string, std::function<void(Registry &)>> initFuncs;
    std::vector<void *> sceneCalls;
    Registry &m_reg;
public:
    SceneManager(Registry &reg) : m_reg(reg) {}
    ~SceneManager() {}

    void setScene(std::string sceneName) {
        m_reg.clear();
       auto func = initFuncs[sceneName];
       func(m_reg);
    }

    template<typename Func>
    void createScene(std::string name, Func &&func) {
        static_assert(std::is_invocable_r_v<void, Func, Registry&>,
                          "func must be callable with Registry& and return void");
        initFuncs[name] = std::function<void(Registry&)>(std::forward<Func>(func));
    }
};
} // namespace vl
