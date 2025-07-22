#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <fkYAML/fkyaml_fwd.hpp>
#include <fkYAML/node.hpp>

#include <memory>
#include <string>
#include <unordered_map>

#include "resourceIDs.h"

namespace vl {

class AssetManager {
private:
  std::unordered_map<
      std::string,
      std::unique_ptr<SDL_Texture *, decltype(&SDL_DestroyTexture)>>
      m_texCache;
  std::unordered_map<std::string,
                     std::unique_ptr<TTF_Font *, decltype(&TTF_CloseFont)>>
      m_fontCache;

  std::unordered_map<TextureID, std::string> m_texMap;
  std::unordered_map<FontID, std::string> m_fontMap;

  bool parseManifest();
  void parseTextures(const fkyaml::node &config);
  void parseFonts(const fkyaml::node &config);

public:
  AssetManager();
};
} // namespace vl
