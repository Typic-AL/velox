#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <vector>

#include "../resourceIDs.h"

namespace vl {

struct TilemapTileset {
  std::string textureId;
  int firstGid = 1;
  int tileWidth = 0;
  int tileHeight = 0;
  int columns = 0;
  int tileCount = 0;
};

struct TilemapLayer {
  std::string name;
  int id = 0;
  int width = 0;
  int height = 0;
  float offsetX = 0.0f;
  float offsetY = 0.0f;
  float opacity = 1.0f;
  bool visible = true;
  std::vector<int> data; // gids, 0 = empty
};

struct TilemapData {
  int tileWidth = 0;
  int tileHeight = 0;
  int mapWidth = 0;
  int mapHeight = 0;
  std::vector<TilemapTileset> tilesets;
  std::vector<TilemapLayer> layers;

  // Returns the tileset that owns the given gid, or nullptr.
  const TilemapTileset *tilesetForGid(int gid) const {
    const TilemapTileset *best = nullptr;
    for (const auto &ts : tilesets) {
      if (gid >= ts.firstGid && (best == nullptr || ts.firstGid > best->firstGid))
        best = &ts;
    }
    return best;
  }

  // Source rect in the tileset texture for the given gid.
  SDL_FRect srcRectForGid(int gid, const TilemapTileset &ts) const {
    int localId = gid - ts.firstGid;
    int col = localId % ts.columns;
    int row = localId / ts.columns;
    return SDL_FRect{
        static_cast<float>(col * ts.tileWidth),
        static_cast<float>(row * ts.tileHeight),
        static_cast<float>(ts.tileWidth),
        static_cast<float>(ts.tileHeight),
    };
  }
};

struct TilemapRenderer {
  TilemapID id;
  int zIndex = 0;
  bool useRenderScale = true;
  // When non-empty, only layers whose name appears in this list are rendered.
  std::vector<std::string> layerFilter;
};

} // namespace vl
