#include "velox/assetManager.h"
#include <charconv>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>

#include "velox/renderWindow.h"

namespace {
// Tiled percent-encodes special characters in file paths (spaces -> %20, etc.).
std::string percentDecode(std::string s) {
  for (size_t i = 0; i + 2 < s.size(); ) {
    if (s[i] == '%') {
      unsigned ch = 0;
      if (std::from_chars(s.data() + i + 1, s.data() + i + 3, ch, 16).ec == std::errc{}) {
        s.replace(i, 3, 1, static_cast<char>(ch));
      }
    }
    ++i;
  }
  return s;
}
} // namespace

namespace vl {
bool AssetManager::parseManifest() {
  try {
    std::ifstream fileStream(m_assetsPath);
    if (!fileStream.is_open()) {
      throw std::runtime_error("[Asset Manager] Failed to open assets file");
    }
    json config = json::parse(fileStream);
    fileStream.close();
    parseTextures(config);
    parseFonts(config);
    parseAnims(config);
    parseTilemaps(config);
    return true;
  } catch (const json::parse_error &e) {
    std::cerr << "[Asset Manager] JSON parsing error for assets.json: "
              << e.what() << "\n";
    return false;
  } catch (const std::exception &e) {
    std::cerr << "[Asset Manager] An unexpected C++ error occurred while "
                 "parsing assets.json: "
              << e.what() << "\n";
    return false;
  }
}

void AssetManager::parseTextures(const json &config) {
  if (!config.contains("textures") || !config["textures"].is_array()) {
    std::cout << "[Asset Manager] No 'textures' section found or it's not a "
                 "sequence\n";
    return;
  }
  const json &texNode = config["textures"];
  for (const auto &texEntry : texNode) {
    if (!texEntry.is_object()) {
      std::cerr
          << "[Asset Manager] Unexpected non-map node in 'textures' entry\n";
      continue;
    }
    if (!texEntry.contains("id") || !texEntry["id"].is_string()) {
      std::cerr << "[Asset Manager] Malformed texture entry (missing an 'id', "
                   "'path', or wrong type)\n";
      continue;
    }
    m_texMap[texEntry["id"].get<std::string>()] =
        texEntry["path"].get<std::string>();
  }
}

void AssetManager::parseFonts(const json &config) {
  if (!config.contains("fonts") || !config["fonts"].is_array()) {
    std::cout << "[Asset Manager] No 'fonts' section found or it's not a "
                 "sequence\n";
    return;
  }
  const json &fontNode = config["fonts"];
  for (const auto &fontEntry : fontNode) {
    if (!fontEntry.is_object()) {
      std::cerr << "[Asset Manager] Unexpected non-map node in 'fonts' entry\n";
      continue;
    }
    if (!fontEntry.contains("id") || !fontEntry["id"].is_string()) {
      std::cerr << "[Asset Manager] Malformed font entry (missing an 'id', "
                   "'path', or wrong type)\n";
      continue;
    }
    m_fontMap[fontEntry["id"].get<std::string>()] =
        fontEntry["path"].get<std::string>();
  }
}

void AssetManager::parseAnims(const json &config) {
  if (!config.contains("animations") || !config["animations"].is_array()) {
    std::cout << "[Asset Manager] No 'animations' section found or it's not a "
                 "sequence\n";
    return;
  }
  const json &animNode = config["animations"];
  for (const auto &animEntry : animNode) {
    if (!animEntry.is_object()) {
      std::cerr << "[Asset Manager] Unexpected non-object node in 'animations' "
                   "entry\n";
      continue;
    }
    if (!animEntry.contains("id") || !animEntry["id"].is_string() ||
        !animEntry.contains("path") || !animEntry["path"].is_string()) {
      std::cerr << "[Asset Manager] Malformed animation entry (missing or "
                   "invalid 'id'/'path')\n";
      continue;
    }
    m_animMap[animEntry["id"].get<std::string>()] =
        animEntry["path"].get<std::string>();
  }
}

SDL_Texture *AssetManager::idToTex(TextureID id) {
  if (m_texCache.find(id) != m_texCache.end())
    return m_texCache[id].get();
  SDL_Surface *surface = IMG_Load(m_texMap[id].c_str());
  if (!surface)
    SDL_Log("[Asset Manager] Unable to load image %s! SDL_image Error: %s\n",
            m_texMap[id].c_str(), SDL_GetError());
  else {
    SDL_Texture *tex =
        SDL_CreateTextureFromSurface(m_renderWindow->getRen(), surface);
    if (!tex) {
      SDL_Log("Unable to create texture from %s! SDL Error: %s\n",
              m_texMap[id].c_str(), SDL_GetError());
    }

    SDL_DestroySurface(surface);
    m_texCache[id] =
        std::unique_ptr<SDL_Texture, decltype(texDeleter)>(tex, texDeleter);
    return tex;
  }
  return nullptr;
}

SDL_Texture *AssetManager::loadTextureFromPath(const std::string &path) {
  // Check cache first — the path itself is the key.
  auto cacheIt = m_texCache.find(path);
  if (cacheIt != m_texCache.end())
    return cacheIt->second.get();

  SDL_Surface *surface = IMG_Load(path.c_str());
  if (!surface) {
    SDL_Log("[Asset Manager] Unable to load image from path %s! SDL_image Error: %s\n",
            path.c_str(), SDL_GetError());
    return nullptr;
  }
  SDL_Texture *tex = SDL_CreateTextureFromSurface(m_renderWindow->getRen(), surface);
  SDL_DestroySurface(surface);
  if (!tex) {
    SDL_Log("[Asset Manager] Unable to create texture from %s! SDL Error: %s\n",
            path.c_str(), SDL_GetError());
    return nullptr;
  }
  m_texCache[path] = std::unique_ptr<SDL_Texture, decltype(texDeleter)>(tex, texDeleter);
  return tex;
}

TTF_Font *AssetManager::idToFont(FontID id, int size) {
  auto key = std::make_pair(id, size);
  auto it = m_fontCache.find(key);
  if (it != m_fontCache.end())
    return it->second.get();
  auto pathIt = m_fontMap.find(id);
  if (pathIt == m_fontMap.end()) {
    SDL_Log("[Asset Manager] Unknown font id: %s\n", id.c_str());
    return nullptr;
  }
  TTF_Font *font = TTF_OpenFont(pathIt->second.c_str(), size);
  if (!font) {
    SDL_Log("[Asset Manager] Failed to open font %s: %s\n",
            pathIt->second.c_str(), SDL_GetError());
    return nullptr;
  }
  m_fontCache[key] =
      std::unique_ptr<TTF_Font, decltype(fontDeleter)>(font, fontDeleter);
  return font;
}

SDL_Texture *AssetManager::getTextTex(const std::string &text, FontID id,
                                           int size, SDL_Color color) {
  TextKey key{text, id, size, color};
  auto it = m_textCache.find(key);
  if (it != m_textCache.end())
    return it->second.get();
  TTF_Font *font = idToFont(id, size);
  if (!font)
    return nullptr;
  SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), text.size(), color);
  if (!surface) {
    SDL_Log("[Asset Manager] Failed to render text \"%s\": %s\n", text.c_str(),
            SDL_GetError());
    return nullptr;
  }
  SDL_Texture *tex =
      SDL_CreateTextureFromSurface(m_renderWindow->getRen(), surface);
  SDL_DestroySurface(surface);
  if (!tex) {
    SDL_Log("[Asset Manager] Failed to create texture for text \"%s\": %s\n",
            text.c_str(), SDL_GetError());
    return nullptr;
  }
  m_textCache[key] =
      std::unique_ptr<SDL_Texture, decltype(texDeleter)>(tex, texDeleter);
  return tex;
}

const SpriteAnimation &AssetManager::idToAnim(AnimID id) {
  if (m_animCache.find(id) != m_animCache.end())
    return m_animCache[id];

  std::string fileID = "";
  std::string animKey = id;
  bool isSet = id.find("::") != std::string::npos;
  if (isSet) {
    size_t pos = id.find("::");
    fileID = id.substr(0, pos);
    animKey = id.substr(pos + 2, id.length());
  }

  std::ifstream file(m_animMap[fileID]);
  if (!file.is_open()) {
    throw std::runtime_error(
        "[Asset Manager] Failed to open animation file for " + id);
  }

  json data = json::parse(file);

  SDL_Texture *sheet = idToTex(data["TextureID"]);
  int rows = data["rows"];
  int cols = data["columns"];

  if (isSet)
    data = data[animKey];

  SpriteAnimation anim;
  anim.fps = data["fps"];
  anim.loop = data["loop"];

  float sizeX, sizeY;

  SDL_GetTextureSize(sheet, &sizeX, &sizeY);
  int rowSize = sizeY / rows;
  int colSize = sizeX / cols;

  for (auto &frameData : data["frames"]) {
    int row = frameData[0];
    int col = frameData[1];

    float posX = colSize * col;
    float posY = rowSize * row;

    anim.frames.emplace_back(posX, posY, colSize, rowSize);
  }

  m_animCache[id] = std::move(anim);

  return m_animCache[id];
}

void AssetManager::parseTilemaps(const json &config) {
  if (!config.contains("tilemaps") || !config["tilemaps"].is_array()) {
    std::cout << "[Asset Manager] No 'tilemaps' section found or it's not a sequence\n";
    return;
  }
  for (const auto &entry : config["tilemaps"]) {
    if (!entry.contains("id") || !entry.contains("path")) {
      std::cerr << "[Asset Manager] Malformed tilemap entry (missing 'id' or 'path')\n";
      continue;
    }
    m_tilemapMap[entry["id"].get<std::string>()] = entry["path"].get<std::string>();
  }
}

const TilemapData &AssetManager::idToTilemap(TilemapID id) {
  auto cacheIt = m_tilemapCache.find(id);
  if (cacheIt != m_tilemapCache.end())
    return cacheIt->second;

  auto mapIt = m_tilemapMap.find(id);
  if (mapIt == m_tilemapMap.end())
    throw std::runtime_error("[Asset Manager] Unknown tilemap id: " + id);

  std::ifstream file(mapIt->second);
  if (!file.is_open())
    throw std::runtime_error("[Asset Manager] Failed to open tilemap file: " + mapIt->second);

  json data = json::parse(file);

  TilemapData tilemap;
  tilemap.tileWidth  = data.value("tilewidth",  0);
  tilemap.tileHeight = data.value("tileheight", 0);
  tilemap.mapWidth   = data.value("width",  0);
  tilemap.mapHeight  = data.value("height", 0);

  // Parse tilesets. Three cases are supported:
  //   1. External JSON tileset (.tsj) via "source" field — parsed here, texture auto-loaded.
  //   2. Embedded tileset with an "image" field — texture auto-loaded relative to the .tmj.
  //   3. Legacy fallback — no "image" field; textureId is the tileset "name" and must be
  //      registered in assets.json manually.
  // XML tilesets (.tsx) are not supported — Tiled exports identical data as JSON (.tsj).
  std::filesystem::path mapDir = std::filesystem::path(mapIt->second).parent_path();

  for (const auto &ts : data.value("tilesets", json::array())) {
    int firstGid = ts.value("firstgid", 1);

    // Determine where the tileset data lives and which directory image paths are relative to.
    json tsData;
    std::filesystem::path baseDir;

    if (ts.contains("source") && ts["source"].is_string()) {
      // External tileset reference — Tiled stores tileset data in a separate file.
      std::string srcRel = percentDecode(ts["source"].get<std::string>());
      std::filesystem::path srcPath = (mapDir / srcRel).lexically_normal();
      std::string ext = srcPath.extension().string();

      if (ext == ".tsx") {
        SDL_Log("[Asset Manager] XML tilesets (.tsx) are not supported. "
                "Convert '%s' to JSON (.tsj) in Tiled or embed the tileset in the map.\n",
                srcRel.c_str());
        continue;
      }
      if (ext != ".tsj") {
        SDL_Log("[Asset Manager] Unknown tileset source format '%s'; only .tsj (JSON) is supported.\n",
                srcRel.c_str());
        continue;
      }

      std::ifstream tsFile(srcPath);
      if (!tsFile.is_open()) {
        SDL_Log("[Asset Manager] Failed to open external tileset file: %s\n",
                srcPath.string().c_str());
        continue;
      }

      json rawTs = json::parse(tsFile);
      // Tiled wraps external tileset data in a "tileset" key with a type marker.
      if (rawTs.contains("tileset") && rawTs.value("type", "") == "tileset")
        tsData = rawTs["tileset"];
      else
        tsData = rawTs;
      baseDir = srcPath.parent_path();
    } else {
      // Embedded tileset — data is inline in the .tmj.
      tsData = ts;
      baseDir = mapDir;
    }

    TilemapTileset tileset;
    tileset.firstGid   = firstGid;
    tileset.tileWidth  = tsData.value("tilewidth",  tilemap.tileWidth);
    tileset.tileHeight = tsData.value("tileheight", tilemap.tileHeight);
    tileset.columns    = tsData.value("columns",    0);
    tileset.tileCount  = tsData.value("tilecount",  0);

    // Resolve and load the tileset texture. If the tileset specifies an "image" field,
    // we auto-load it relative to the tileset file's directory and use the resolved
    // path as the textureId cache key — no assets.json entry required.
    if (tsData.contains("image") && tsData["image"].is_string()) {
      std::string imgRel = percentDecode(tsData["image"].get<std::string>());
      std::filesystem::path imgPath = (baseDir / imgRel).lexically_normal();
      std::string imgStr = imgPath.string();

      if (loadTextureFromPath(imgStr)) {
        tileset.textureId = imgStr;  // path-keyed lookup in m_texCache
      } else {
        SDL_Log("[Asset Manager] Failed to auto-load tileset texture: %s. "
                "Falling back to tileset name '%s'.\n",
                imgStr.c_str(), tsData.value("name", "").c_str());
        tileset.textureId = tsData.value("name", "");
      }
    } else {
      // No image field — legacy contract: tileset "name" must be registered in assets.json.
      tileset.textureId = tsData.value("name", "");
    }

    tilemap.tilesets.push_back(std::move(tileset));

	    // Build collision map from tile objectgroups.
	    for (const auto &tile : tsData.value("tiles", json::array())) {
      int tileId = tile.value("id", -1);
      if (tileId < 0 || !tile.contains("objectgroup"))
        continue;

      int gid = firstGid + tileId;
      auto &rects = tilemap.tileCollisionMap[gid];

      const auto &objects = tile["objectgroup"].value("objects", json::array());
      for (const auto &obj : objects) {
        float ox = obj.value("x", 0.0f);
        float oy = obj.value("y", 0.0f);
        float ow = obj.value("width", 0.0f);
        float oh = obj.value("height", 0.0f);
        if (ow > 0 && oh > 0)
          rects.push_back(SDL_FRect{ox, oy, ow, oh});
      }
    }
  }

  for (const auto &layer : data.value("layers", json::array())) {
    // Only tile layers (type == "tilelayer") are supported for rendering.
    if (layer.value("type", "") != "tilelayer")
      continue;

    TilemapLayer tl;
    tl.name    = layer.value("name",    "");
    tl.id      = layer.value("id",      0);
    tl.width   = layer.value("width",   0);
    tl.height  = layer.value("height",  0);
    tl.offsetX = layer.value("offsetx", 0.0f);
    tl.offsetY = layer.value("offsety", 0.0f);
    tl.opacity = layer.value("opacity", 1.0f);
    tl.visible = layer.value("visible", true);

    if (layer.contains("data") && layer["data"].is_array())
      tl.data = layer["data"].get<std::vector<int>>();

    tilemap.layers.push_back(std::move(tl));
  }

  m_tilemapCache[id] = std::move(tilemap);
  return m_tilemapCache[id];
}

} // namespace vl
