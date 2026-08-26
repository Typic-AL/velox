#pragma once

#include <SDL3/SDL.h>

namespace vl {

namespace Time {
inline int targetFPS = 144;

inline Uint64 previousTime = SDL_GetPerformanceCounter();
inline Uint64 currentTime = SDL_GetPerformanceCounter();

inline float accumulator = 0.0f;
inline float alpha = 0.0f;

inline float deltaTime = 0.0f;

inline float secondsSince(Uint64 start, Uint64 end) {
  return static_cast<float>(end - start) /
         static_cast<float>(SDL_GetPerformanceFrequency());
}

inline void beginFrame() {
  currentTime = SDL_GetPerformanceCounter();
  float frameTime = secondsSince(previousTime, currentTime);
  deltaTime = frameTime;
  previousTime = currentTime;

  // Clamp to avoid spiral of death
  const float maxFrameTime = 0.25f;
  accumulator += frameTime < maxFrameTime ? frameTime : maxFrameTime;
}

inline void delayIfNeeded() {
  Uint64 frameEnd = SDL_GetPerformanceCounter();

  Uint32 desiredMS = static_cast<Uint32>(1000.0f / targetFPS);
  Uint32 elapsedMS =
      static_cast<Uint32>(secondsSince(currentTime, frameEnd) * 1000.0f);

  if (elapsedMS < desiredMS) {
    SDL_Delay(desiredMS - elapsedMS);
  }
}

inline void endFrame() { delayIfNeeded(); }

} // namespace Time

inline void setTargetFPS(int fps) { Time::targetFPS = fps; }

namespace Physics {
inline int targetFPS = 60;
inline float fixedDeltaTime = 1.0f / targetFPS;

inline bool shouldUpdate() {
  if (Time::accumulator >= fixedDeltaTime) {
    Time::accumulator -= fixedDeltaTime;
    return true;
  }
  return false;
}

inline void stepPhysics() {
  float a = Time::accumulator / fixedDeltaTime;
  Time::alpha = a < 0.0f ? 0.0f : (a > 1.0f ? 1.0f : a);
}
} // namespace Physics

inline void setPhysicsFPS(int fps) {
  Physics::targetFPS = fps;
  Physics::fixedDeltaTime = 1.0f / fps;
}
} // namespace vl
