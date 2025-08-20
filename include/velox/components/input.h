#pragma once

#include <SDL3/SDL.h>

#include <vector>

struct Input {
    int numKeys;
    const bool *keyboard;
    std::vector<bool> justPressedKeys;
    std::vector<bool> justReleasedKeys;

    Input() {
        keyboard = SDL_GetKeyboardState(&numKeys);

        justPressedKeys.assign(numKeys, false);
        justReleasedKeys.assign(numKeys, false);
    }
};
