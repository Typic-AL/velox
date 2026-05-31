#include "velox/components/tilemap.h"
#include "velox/assetManager.h"
#include "velox/components/collider.h"
#include "velox/components/core.h"

namespace vl {

void createTilemapColliders(Registry &reg, TilemapID id, int collisionLayer) {
  auto *assetMan = reg.tryGetResource<AssetManager>();
  if (!assetMan)
    return;

  const TilemapData &data = assetMan->idToTilemap(id);
  if (data.tileCollisionMap.empty())
    return;

  int tw = data.tileWidth;
  int th = data.tileHeight;
  int mw = data.mapWidth;
  int mh = data.mapHeight;

  std::vector<std::vector<bool>> solid(mh, std::vector<bool>(mw, false));

  for (const auto &layer : data.layers) {
    for (int row = 0; row < layer.height; ++row) {
      for (int col = 0; col < layer.width; ++col) {
        int idx = row * layer.width + col;
        if (idx >= static_cast<int>(layer.data.size()))
          continue;

        int rawGid = layer.data[idx];
        if (rawGid == 0)
          continue;

        int gid = rawGid & 0x1FFFFFFF;
        auto it = data.tileCollisionMap.find(gid);
        if (it == data.tileCollisionMap.end())
          continue;

        bool hasFullTile = false;
        for (const auto &r : it->second) {
          bool isFullTile =
              (r.x == 0.0f && r.y == 0.0f && static_cast<int>(r.w) == tw &&
               static_cast<int>(r.h) == th);
          if (isFullTile) {
            hasFullTile = true;
          } else {
            // Partial shape — emit directly
            float worldX = col * tw + r.x;
            float worldY = row * th + r.y;
            Entity e = reg.createWith(
                Transform(glm::vec2(worldX, worldY)),
                Rigidbody{.type = BodyType::STATIC},
                Collider{.width = r.w, .height = r.h, .layer = collisionLayer});
            (void)e;
          }
        }
        if (hasFullTile)
          solid[row][col] = true;
      }
    }
  }

  for (int row = 0; row < mh; ++row) {
    for (int col = 0; col < mw; ++col) {
      if (!solid[row][col])
        continue;

      float worldX = col * tw;
      float worldY = row * th;
      Entity e = reg.createWith(Transform(glm::vec2(worldX, worldY)),
                                Rigidbody{.type = BodyType::STATIC},
                                Collider{.width = static_cast<float>(tw),
                                         .height = static_cast<float>(th),
                                         .layer = collisionLayer});
      (void)e;
    }
  }
}

} // namespace vl
