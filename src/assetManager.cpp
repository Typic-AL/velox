#include "velox/assetManager.h"
#include "fkYAML/fkyaml_fwd.hpp"
#include <fstream>
#include <iostream>
#include <memory>

#include "nlohmann/json_fwd.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

#include "velox/renderWindow.h"

namespace vl {

bool AssetManager::parseManifest() {
  try {
    std::ifstream fileStream(m_assetsPath);
    if (!fileStream.is_open()) {
      throw std::runtime_error("[Asset Manager] Failed to open assets file");
    }

    fkyaml::node config = fkyaml::node::deserialize(fileStream);
    fileStream.close();

    parseTextures(config);
    parseFonts(config);
    parseAnims(config);

    return true;
  } catch (const fkyaml::exception &e) {
    std::cerr << "[Asset Manager] fkYAML parsing error for assets.yml: "
              << e.what() << "\n";
    return false;
  } catch (const std::exception &e) {
    std::cerr << "[Asset Manager] An unexpected C++ error occurred while "
                 "parsing assets.yml: "
              << e.what() << "\n";
    return false;
  }
}

void AssetManager::parseTextures(const fkyaml::node &config) {
  if (!config.contains("textures") || !config["textures"].is_sequence()) {
    std::cout << "[Asset Manager] No 'textures' section found or it's not a "
                 "sequence\n";
    return;
  }

  const fkyaml::node &texNode = config["textures"];
  for (const auto &texEntry : texNode) {
    if (!texEntry.is_mapping()) {
      std::cerr
          << "[Asset Manager] Unexpected non-map node in 'textures' entry\n";
      continue;
    }
    if (!texEntry.contains("id") || !texEntry["id"].is_string()) {
      std::cerr << "[Asset Manager] Malformed texture entry (missing an 'id', "
                   "'path', or wrong type)\n";
      continue;
    }

    m_texMap[texEntry["id"].get_value<std::string>()] =
        texEntry["path"].get_value<std::string>();
  }
}

void AssetManager::parseFonts(const fkyaml::node &config) {
  if (!config.contains("fonts") || !config["fonts"].is_sequence()) {
    std::cout << "[Asset Manager] No 'fonts' section found or it's not a "
                 "sequence\n";
    return;
  }

  const fkyaml::node &fontNode = config["fonts"];
  for (const auto &fontEntry : fontNode) {
    if (!fontEntry.is_mapping()) {
      std::cerr << "[Asset Manager] Unexpected non-map node in 'fonts' entry\n";
      continue;
    }
    if (!fontEntry.contains("id") || !fontEntry["id"].is_string()) {
      std::cerr << "[Asset Manager] Malformed font entry (missing an 'id', "
                   "'path', or wrong type)\n";
      continue;
    }

    m_fontMap[fontEntry["id"].get_value<std::string>()] =
        fontEntry["path"].get_value<std::string>();
  }
}

void AssetManager::parseAnims(const fkyaml::node &config) {
    if (!config.contains("animations") || !config["animations"].is_sequence()) {
        std::cout << "[Asset Manager] No 'animations' section found or it's not a "
                     "sequence\n";
        return;
      }

      const fkyaml::node &fontNode = config["animations"];
      for (const auto &animEntry : fontNode) {
        if (!animEntry.is_mapping()) {
          std::cerr << "[Asset Manager] Unexpected non-map node in 'animations' entry\n";
          continue;
        }
        if (!animEntry.contains("id") || !animEntry["id"].is_string()) {
          std::cerr << "[Asset Manager] Malformed animation entry (missing an 'id', "
                       "'path', or wrong type)\n";
          continue;
        }

        m_animMap[animEntry["id"].get_value<std::string>()] =
            animEntry["path"].get_value<std::string>();
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

TTF_Font *idToFont(FontID id, int size) { return nullptr; }

const SpriteAnimation &AssetManager::idToAnim(AnimID id) {
    if(m_animCache.find(id) != m_animCache.end())
        return m_animCache[id];

    std::string fileID = "";
    std::string animKey = id;
    bool isSet = id.find("::") != std::string::npos;
    if(isSet) {
        size_t pos = id.find("::");
        fileID = id.substr(0, pos);
        animKey = id.substr(pos + 2, id.length());
    }

    std::ifstream file(m_animMap[fileID]);
    if (!file.is_open()) {
          throw std::runtime_error("[Asset Manager] Failed to open animation file for " + id);
    }

    json data = json::parse(file);

    SDL_Texture *sheet = idToTex(data["TextureID"]);
    int rows = data["rows"];
    int cols = data["columns"];

    if(isSet)
        data = data[animKey];

    SpriteAnimation anim;
    anim.fps = data["fps"];
    anim.loop = data["loop"];

    float sizeX, sizeY;

    SDL_GetTextureSize(sheet, &sizeX, &sizeY);
    int rowSize = sizeY / rows;
    int colSize = sizeX / cols;

    for (auto& frameData : data["frames"]) {
        int row = frameData[0];
        int col = frameData[1];

        float posX = colSize * col;
        float posY = rowSize * row;

        anim.frames.emplace_back(posX, posY, colSize, rowSize);
    }

    m_animCache[id] = std::move(anim);

    return m_animCache[id];
}

} // namespace vl
