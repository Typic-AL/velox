#pragma once

#include "../resourceIDs.h"
#include "../entity.h"
#include "../registry.h"
#include <vector>

using Frame = SDL_FRect;

namespace vl {

struct SpriteAnimation {
    int fps;
    bool loop = false;
    std::vector<Frame> frames;
};

struct SpriteAnimator {
    AnimID currentAnim;
    int currentFrame = 0;
    float frameTimer = 0.0f;
    bool isPlaying = true;
    bool complete = false;
};

void setAnim(Registry &reg, Entity e, AnimID newAnim);
}
