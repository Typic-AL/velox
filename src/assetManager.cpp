#include "velox/assetManager.h"
#include "fkYAML/fkyaml_fwd.hpp"
#include <fstream>
#include <iostream>
#include <memory>

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

} // namespace vl
