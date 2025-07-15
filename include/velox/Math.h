#pragma once

#include <SDL3/SDL.h>
#include <algorithm>
#include <chrono>
#include <math.h>

namespace vl {

struct Vec2 {
  float x = 0.0f;
  float y = 0.0f;

  Vec2 &operator+=(const Vec2 &other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  Vec2 &operator-=(const Vec2 &other) {
    x -= other.x;
    y -= other.y;
    return *this;
  }

  Vec2 &operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
  }

  Vec2 &operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
  }

  Vec2 normalized() {
    float length = sqrt(x * x + y * y);
    if (length == 0.0f)
      return {0.0f, 0.0f};
    return {x / length, y / length};
  }

  float getLength() { return sqrt(x * x + y * y); }
};

inline Vec2 operator+(Vec2 a, Vec2 b) { return {a.x + b.x, a.y + b.y}; }
inline Vec2 operator-(Vec2 a, Vec2 b) { return {a.x - b.x, a.y - b.y}; }
inline Vec2 operator*(Vec2 a, Vec2 b) { return {a.x * b.x, a.y * b.y}; }
inline Vec2 operator/(Vec2 a, Vec2 b) { return {a.x / b.x, a.y / b.y}; }

inline Vec2 operator*(Vec2 a, float b) { return {a.x * b, a.y * b}; }
inline Vec2 operator*(float a, Vec2 b) { return {a * b.x, a * b.y}; }
inline Vec2 operator/(Vec2 a, float b) { return {a.x / b, a.y / b}; }

inline bool operator==(Vec2 a, Vec2 b) { return a.x == b.x && a.y == b.y; }
inline bool operator!=(Vec2 a, Vec2 b) { return a.x != b.x || a.y != b.y; }
inline bool operator<(Vec2 a, Vec2 b) { return a.x < b.x && a.y < b.y; }
inline bool operator>(Vec2 a, Vec2 b) { return a.x > b.x && a.y > b.y; }
inline bool operator<=(Vec2 a, Vec2 b) { return a.x <= b.x && a.y <= b.y; }
inline bool operator>=(Vec2 a, Vec2 b) { return a.x >= b.x && a.y >= b.y; }

inline Vec2 vec2Lerp(const Vec2 &start, const Vec2 &end, float alpha) {
  return {start.x + (end.x - start.x) * alpha,
          start.y + (end.y - start.y) * alpha};
}

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
