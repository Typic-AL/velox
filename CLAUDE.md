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

Output is a static library `libvelox`. There are no tests or lint targets currently.

Dependencies: SDL3, SDL3_image, SDL3_ttf (system-installed). Vendored: GLM, nlohmann/json, tinyxml2.

## Architecture Overview

Velox is a 2D game engine built on an **Entity Component System (ECS)** pattern with SDL3 as the backend.

### ECS Core

- **Entity** (`include/velox/entity.h`): `unsigned int` identifier
- **Registry** (`include/velox/registry.h`): Central manager — creates entities, stores components via sparse sets, holds resources (singletons like `AssetManager`, `Input`), and runs systems. Template-heavy for compile-time type safety.
- **SparseSet** (`include/velox/sparseSet.h`): Cache-friendly component storage; each component type gets its own pool.
- **ComponentStorage** (`include/velox/componentStorage.h`): Type-erased pool abstraction used by Registry.

### Game Loop (engine.cpp)

`Engine::update()` runs on a fixed physics timestep (accumulator-based). `Engine::render()` runs every frame. Positions are interpolated between physics steps for smooth visuals.

**Update order**: input → buttons → registered systems → velocity → collision → physics step  
**Render order**: animate → collect render commands → present

### Components (pure data, in `include/velox/components/`)

- `core.h`: `Transform` (pos + prev pos for interpolation), `Rigidbody` (velocity, body type), `SpriteRenderer`, `TextRenderer`, `RectRenderer`
- `collider.h`: `Collider` — AABB with layer mask bitmask and enter/exit/stay callbacks
- `animation.h`: `SpriteAnimator` — frame index and timer; driven by `AssetManager` animation data
- `ui.h`: `Button`, `NineSlice`
- `tilemap.h`: `TilemapRenderer` (Tiled format via tinyxml2)

### Systems (functions, in `include/velox/systems/`)

Systems are registered with the Registry and called each update/render cycle. They query the Registry for entities that have specific component combinations (views return tuples).

- `renderSystem.h/cpp`: Collects draw commands from all drawable components, sorts by z-index, handles UI vs. game-world scaling
- `collisionSystem.h/cpp`: Sweep & prune AABB detection; invokes collision callbacks
- `animationSystem.h` / `animation.cpp`: Advances frame indices based on elapsed time
- `systems/core.h`: Velocity application and position interpolation

### Asset Pipeline

`AssetManager` (`include/velox/assetManager.h`, `src/assetManager.cpp`) loads from `assets.json` at startup. Resource IDs are `std::string` aliases (`TextureID`, `FontID`, `AnimID`, `TilemapID` — see `include/velox/resourceIDs.h`). Rendered text is cached by (font, size, color, string) key.

### Key Design Patterns

- **Resources via Registry**: Singletons (AssetManager, Input, etc.) are stored as resources in the Registry rather than global variables.
- **Layer masks**: Collision filtering uses bitmasks on `Collider` components.
- **Z-index + UI flag**: Rendering order is controlled per-component; UI components render in screen space.
- **Fixed timestep with interpolation**: Physics runs at a configurable Hz (default 60); `Transform` stores previous position so the render system can interpolate.


Do not make any changes until you have 95% confidence in what you need to build. Ask me follow-up questions until you reach that confidence.
