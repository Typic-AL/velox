#pragma once

#include <glm/glm.hpp>

namespace vl {

class Registry;

enum class Space { WORLD, SCREEN };

glm::vec2 cameraOffset(Registry &reg);
glm::vec2 worldToScreen(Registry &reg, glm::vec2 world);
glm::vec2 screenToWorld(Registry &reg, glm::vec2 screen);

} // namespace vl
