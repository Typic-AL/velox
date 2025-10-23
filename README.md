# Velox Engine

<div align="center">

### A performance-focused 2D game engine built with C++ and SDL3

[![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)]()
[![Status](https://img.shields.io/badge/Status-In_Development-yellow?style=for-the-badge)]()

*Currently under active development*

</div>

---

## Overview

A modern 2D game engine built from the ground up in C++, leveraging Entity Component System (ECS) architecture for maximum performance and flexibility. The goal of this engine is to provide the foundation to create a wide variety of 2d games.

---

## Architecture

**Entity Component System (ECS)**

The engine is built on ECS principles, separating data from behavior for optimal performance:

- **Entities**: Lightweight identifiers for game objects
- **Components**: Pure data containers with no logic
- **Systems**: Process entities with specific component combinations

This architecture enables:
- Cache-friendly data layout
- Easy parallelization opportunities
- Flexible composition of game objects
- Minimal memory overhead
  
Not every aspect of this engine has logic separted from data however. Classes such as the engine, asset manager, and registry are designed this way to make using them a lot more straightforward, especially because of how often these are designed to be used when creating a game.

---

## Required Dependencies
- SDL3
- SDL3_image
- SDL3_ttf

---

## Building

**This engine is designed to be used as a Meson subproject using the Meson build system**

***Currently building only works on Linux and macOS***

Add this engine to your project's subprojects directory - symlink is fine too:

```bash
git clone https://github.com/Typic-AL/velox subprojects/velox
```
Then in your meson.build:
```meson
sdl_dep = dependency('sdl3')
image_dep = dependency('sdl3-image')
ttf_dep = dependency('sdl3-ttf')

velox_dep = subproject('velox').get_variable('velox_dep')

src = ['main.cpp']
executable('your_game',
  src,
  dependencies: [sdl_dep, image_dep, ttf_dep, velox_dep]
)
```
Then to actually build your game simply do:
```bash
mkdir build
meson setup build
meson compile -C build
```
---

## Basic Game Loop

```c++
#include <velox/engine.h>

int main(int arc, char *argv[]) {
  vl::Engine engine;
  engine.init("My Game", 1280, 720);

  engine.setMaxFPS(144); // max fps is 60 by default
  engine.setPhysicsFPS(60); // physics fps is also 60 by default

  bool running = true;
  SDL_Event event;

  while(running) {
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_EVENT_QUIT)
        running = false;
    }

    engine.beginFrame();

    engine.update();
    engine.render();

    engine.endFrame();
  }

}
```
---

## Current Features

- Basic rendering system using the SDL renderer pipeline
- Simple asset pipeline using an assets.yml config file
- An AABB collision system with customizable behavior on collision
- Simple sprite animations customized using json files
- Scene system
- Simple input system for detecting key presses and mouse updates

---

## Project Status

This engine is currently **in development**. Core systems are functional but the feature set is being actively expanded.

### Planned Features

- Audio system
- Particle effects
- Tilemap support via Tiled
- Serialization system
- UI system

---



## Technical Details

**Language**: C++

**Libraries**: SDL3, fkYAML, nlohmann json

**Architecture**: Entity Component System

---

## Contributing

This is a personal learning project and is not currently accepting contributions. However, feedback and suggestions are always welcome.

---

## Roadmap

Development priorities are focused on:
1. Completing core engine systems
2. Performance optimization
3. Documentation and examples

---
<div align="center">

</div>
