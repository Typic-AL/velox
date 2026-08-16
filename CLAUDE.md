# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
# Initial setup
meson setup build

# Build
meson compile -C build

# Clean rebuild
rm -rf build && meson setup build && meson compile -C build
```

Output is a static library `libvelox` plus `velox_dep` for consumers. Velox is designed to be used as a **Meson subproject** by a game project (see README for the consumer-side `meson.build`). There are no tests or lint targets.

New `src/*.cpp` files must be added to the `src` list in `meson.build` or they won't be compiled/linked.

Dependencies (system-installed via pkg-config): SDL3, SDL3_image, SDL3_ttf, SDL3_mixer. Vendored under `include/`: GLM, nlohmann/json. `compile_commands.json` symlinks into `build/`.

## Architecture Overview

Velox is a 2D game engine built on an **Entity Component System (ECS)** with SDL3 as the backend. Everything lives in namespace `vl`. ECS applies to *game* logic only — engine infrastructure (Engine, RenderWindow, AssetManager, AudioMixer, Input) is plain classes.

### ECS Core

- **Entity** (`entity.h`): an `unsigned int` id; groups components, holds no data itself.
- **Registry** (`registry.h`): the hub. Creates entities, stores components in per-type sparse sets, holds **resources** (singletons), and runs **systems**. Heavily templated for compile-time type safety. Key APIs:
  - `createWith(components...)`, `addComponent<T>`, `get<T>`, `has<T>`
  - `view<A, B>()` returns tuples of component refs; chain `.with<Tag>()` to further filter, `.withEntity()` to also yield the `Entity`.
  - `registerSystem(func, args...)` — `func` must be `void(Registry&, args...)`; run via `runSystems()`.
  - Resources: `emplaceResource<T>` (owning), `setResource<T>` (non-owning ref), `getResource<T>`, `tryGetResource<T>`.
  - `clear()` wipes entities, systems, and component storages (used on scene switch).
- **SparseSet** (`sparseSet.h`) / **ComponentStorage** (`componentStorage.h`): cache-friendly per-type component pools; `ComponentPool` is the type-erased base the Registry stores.

### Game Loop (`engine.cpp`)

`Engine` owns the window, asset manager, audio mixer, registry, scene manager, and input, and registers each as a Registry resource in its constructor. The consumer drives the loop manually (`beginFrame` / `update` / `render` / `endFrame`) — see README.

- **`update()`** runs a fixed-timestep accumulator loop (`Physics::shouldUpdate()`), each iteration: update mouse → `handleButtonPresses` → `runSystems` (user systems) → `applyVelocity` → `sweepAndPrune` (collision). After the loop: `stepPhysics`, then `interpolatePosition`, `updateCamera`, `updateProgressBars`, and `Input::reset`.
- **`render()`** runs once per frame: clear → `animateSprites` → `renderSystem` → present.
- Physics runs at a configurable Hz (default 60). `Transform` stores `pos`, `prevPos`, and `lPos` (interpolated) so rendering stays smooth between physics steps.

### Scenes (`scene.h`)

`SceneManager` maps names to `void(Registry&)` init functions (`createScene`). `setScene` calls `Registry::clear()` then the init function, rebuilding all entities/systems for that scene.

### Components (pure data)

Live in `include/velox/components/`. **Do not add new component types to `core.h` or existing files — create a new header under `components/`.**

- `core.h`: `Transform`, `Rigidbody` (`BodyType` STATIC/DYNAMIC/KINEMATIC), `SpriteRenderer`, `TextRenderer`, `RectRenderer`.
- `collider.h`: `Collider` — AABB with layer-mask bitmask and enter/exit/stay callbacks.
- `animation.h`: `SpriteAnimator` — frame index + timer, driven by `AssetManager` animation data.
- `audioSource.h`: `AudioSource` + free `playSound(reg, mixer, source, oneShot)` helper.
- `tilemap.h`: `TilemapRenderer` (Tiled `.tmj` format).
- `ui/` (`ui.h` aggregates `control.h`, `layout.h`, `visual.h`): buttons, progress bars, nine-slice, anchoring.

### Systems

In `include/velox/systems/`. Systems are free functions taking `Registry&`; they `view<...>()` matching entities. **Add new systems as new headers under `systems/`, not to `core.h`.**

- `renderSystem.h`/`renderSystem.cpp`: collects draw commands from all drawables, sorts by z-index, handles space/scaling. Uses a `RenderContext` (window + asset manager).
- `collisionSystem.h`/`collisionSystem.cpp`: sweep-and-prune AABB; fires collider callbacks.
- `animationSystem.h`/`animation.cpp`: advances animation frames by elapsed time.
- `core.h`: `applyVelocity` and `interpolatePosition`.

### Coordinate Spaces (`space.h`)

`Space` enum is `WORLD` or `SCREEN`, set per renderable component (this replaced the old `isUi` flag). WORLD components are offset by the camera; SCREEN components render in fixed screen space. Helpers: `cameraOffset`, `worldToScreen`, `screenToWorld`.

### Asset Pipeline

`AssetManager` (`assetManager.h`/`.cpp`) loads from `assets.json` at startup (`parseManifest`). Resource ids are `std::string` aliases in `resourceIDs.h` (`TextureID`, `FontID`, `AnimID`, `TilemapID`, `AudioID`), letting `assets.json` abbreviate file paths into ids. Rendered text is cached by (font, size, color, string).

### Audio (`audioMixer.h`)

`AudioMixer` wraps SDL3_mixer: `initAudio` opens a `MIX_Mixer`, and it owns a pool of `MIX_Track`s handed out by `getFreeTrack`. Play sounds via the `playSound` helper on an `AudioSource`.

### Key Design Patterns

- **Resources over globals**: singletons live in the Registry, fetched by type.
- **Layer masks**: collision filtering via bitmasks on `Collider`.
- **Fixed timestep + interpolation**: physics Hz independent of render FPS; `Transform.lPos` is what the renderer draws.

## Code Style

- No inline comments unless the logic is genuinely non-obvious.
- No docstrings on simple/self-evident functions.
- No section-header comments (`// Initialize variables`) or comments that restate the code.
- Name things so comments aren't needed. When a comment is warranted, keep it terse — not a full punctuated sentence.
