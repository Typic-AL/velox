#pragma once

#include <SDL3/SDL.h>
#include <chrono>

namespace vl {

namespace Time {
using Clock = std::chrono::high_resolution_clock;
using Duration = std::chrono::duration<float>;
using TimePoint = std::chrono::time_point<Clock>;

inline int targetFPS = 144;

inline TimePoint previousTime = Clock::now();
inline TimePoint currentTime = Clock::now();

inline float accumulator = 0.0f;
inline float alpha = 0.0f;

inline float deltaTime = 0.0f;

inline void beginFrame() {
  currentTime = Clock::now();
  Duration frameTime = currentTime - previousTime;
  deltaTime = frameTime.count();
  previousTime = currentTime;

  // Clamp to avoid spiral of death
  const float maxFrameTime = 0.25f;
  accumulator += std::min(frameTime.count(), maxFrameTime);
}

inline void delayIfNeeded() {
  auto frameEnd = Clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
      frameEnd - currentTime);

  Uint32 desiredMS = static_cast<Uint32>(1000.0f / targetFPS);
  Uint32 elapsedMS = static_cast<Uint32>(elapsed.count());

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
  Time::alpha = std::clamp(Time::accumulator / fixedDeltaTime, 0.0f, 1.0f);
}
} // namespace Physics

inline void setPhysicsFPS(int fps) {
  Physics::targetFPS = fps;
  Physics::fixedDeltaTime = 1.0f / fps;
}
} // namespace vl
