#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "components/animation.h"

#include <fkYAML/fkyaml_fwd.hpp>
#include <fkYAML/node.hpp>

#include <memory>
#include <string>
#include <unordered_map>

#include "resourceIDs.h"
#include "util.h"

namespace vl {

class RenderWindow;

inline auto texDeleter = [](SDL_Texture *tex) {
  if (tex)
    SDL_DestroyTexture(tex);
};

inline auto fontDeleter = [](TTF_Font *font) {
  if (font)
    TTF_CloseFont(font);
};

class AssetManager {
private:
  std::unordered_map<TextureID,
                     std::unique_ptr<SDL_Texture, decltype(texDeleter)>>
      m_texCache;
  std::unordered_map<std::pair<FontID, int>,
                     std::unique_ptr<TTF_Font, decltype(fontDeleter)>,
                     PairHash<FontID, int>>
      m_fontCache;
  std::unordered_map<AnimID, SpriteAnimation> m_animCache;

  std::unordered_map<TextureID, std::string> m_texMap;
  std::unordered_map<FontID, std::string> m_fontMap;
  std::unordered_map<AnimID, std::string> m_animMap;

  std::string m_assetsPath = "assets.yml";

  RenderWindow *m_renderWindow = nullptr;

  void parseTextures(const fkyaml::node &config);
  void parseFonts(const fkyaml::node &config);
  void parseAnims(const fkyaml::node &config);

public:
  AssetManager(RenderWindow *renderWindow) : m_renderWindow(renderWindow) {}

  void setAssetsPath(std::string path) { m_assetsPath = path; }
  bool parseManifest();

  SDL_Texture *idToTex(TextureID id);
  SDL_Texture *loadTex(std::string_view path);
  TTF_Font *idToFont(FontID id, int size);
  const SpriteAnimation &idToAnim(AnimID id);
};
} // namespace vl
