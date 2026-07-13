# Velox Engine

<div align="center">

### A 2D game engine built with C++ and SDL3

[![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)]()
[![Status](https://img.shields.io/badge/Status-In_Development-yellow?style=for-the-badge)]()

</div>

## Overview

Velox is my attempt at making a 2D game engine in C++. It's built on top of SDL3 and uses an Entity Component System (ECS), which is known for cache locality and general performance, hence the name "velox", meaning quick in latin.

The main goal of this project is to create a tool that can be used to create games using C++ all in the comfort of your IDE. It's meant to essentially be a framework, except you don't have to waste time creating basic systems that virtually every game needs, such as collisions, asset management, rendering pipelines, tilemaps, sound effects, you get the idea.

## Architecture

**Entity Component System (ECS)**

- **Entities:** a number used to group components together, essentially a GameObject from Unity, but it doesn't actually contain any components. It's more that components are associated with/attached to it
- **Components:** pure data structs that are "attached" to an entity, for example a sprite renderer or collider
- **Systems:** functions that process entities matching a set of components, instead of creating a class with a polymorphic update function, you create a system instead

Component storage uses sparse sets, and the `Registry` also allows for the storage of resources(which are effectively singletons) such as the `AssetManager` and `Input`.

Not every part of the engine is created inside the ECS, for example asset manager, engine class, input, etc. ECS only applies to _game_ logic, not all engine code

## Required Dependencies (for now)

- SDL3
- SDL3_image
- SDL3_ttf
- SDL3_mixer

## Building

**This engine is currently designed to be used as a Meson subproject.**

**_Building has only been tested on MacOS and Linux_**

Add the engine to your project's subprojects directory, you can also use a symlink if you want:

```bash
cd your_game/directory
git clone https://github.com/Typic-AL/velox subprojects/velox
```

Then in your `meson.build`:

```meson
project(
  'your_game',
  'cpp',
  version: '1',
  meson_version: '>= 1.3.0',
  default_options: ['cpp_std=c++20'],
)

velox_dep = subproject('velox').get_variable('velox_dep')

src = ['main.cpp']
executable('your_game',
  src,
  dependencies: [velox_dep]
)
```

`velox_dep` includes velox, as well as all of the SDL3 dependencies

Then build your game:

```bash
meson setup build
meson compile -C build
```

## Basic Game Loop

```cpp
#include <velox/engine.h>

int main(int argc, char *argv[]) {
  vl::Engine engine;
  engine.init("My Game", 1280, 720);

  engine.setMaxFPS(144);     // default is 60
  engine.setPhysicsFPS(60);  // default is 60

  bool running = true;
  SDL_Event event;

  while (running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        running = false;
    }

    engine.beginFrame();

    engine.update();
    engine.render();

    engine.endFrame();
  }
}
```

## Current Features

- Physics loop with a separate physics framerate and physics interpolation
- AABB collision system using sweep and prune, which has layer masks and enter/exit/stay callbacks
- Sprite, text, and rectangle rendering with z-index ordering
- Screen-space vs. world-space rendering per component
- Sprite animations via `.json` files (will probably be reworked at some point)
- Tiled tilemap support using the `.tmj` format, including rendering and also tilemap collisions
- Basic camera with target following, deadzone, smoothing, and bounds clamping
- Minimal, subject to change, UI system with buttons, progress bars, nine slice, and anchoring
- Asset pipeline using an `assets.json` config, allowing for file paths to be abbreviated into asset IDs
- Simple audio system allowing for playback of .mp3, .wav, etc. through a simple playSound() function

## Project Status

This engine is clearly **still in development**. Most core systems are functional, but there are definitely things that need to be added/improved still.

### Planned Features

- More UI elements/UI overhaul
- Refined audio system
- Particle effects
- Serialization system

## Contributing

This is a personal learning project and is not currently accepting contributions. Feedback and suggestions are always welcome.
