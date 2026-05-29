#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "components/animation.h"

#include <memory>
#include <string>
#include <unordered_map>

#include "resourceIDs.h"
#include "util.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

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

  struct TextKey {
    std::string text;
    FontID fontId;
    int size;
    SDL_Color color;
    bool operator==(const TextKey &o) const {
      return text == o.text && fontId == o.fontId && size == o.size &&
             color.r == o.color.r && color.g == o.color.g &&
             color.b == o.color.b && color.a == o.color.a;
    }
  };
  struct TextKeyHash {
    size_t operator()(const TextKey &k) const {
      size_t h = std::hash<std::string>{}(k.text);
      h ^= std::hash<std::string>{}(k.fontId) + 0x9e3779b9 + (h << 6) + (h >> 2);
      h ^= std::hash<int>{}(k.size) + 0x9e3779b9 + (h << 6) + (h >> 2);
      h ^= std::hash<uint32_t>{}((k.color.r << 24) | (k.color.g << 16) |
                                  (k.color.b << 8) | k.color.a) +
           0x9e3779b9 + (h << 6) + (h >> 2);
      return h;
    }
  };
  std::unordered_map<TextKey, std::unique_ptr<SDL_Texture, decltype(texDeleter)>,
                     TextKeyHash>
      m_textCache;

  std::unordered_map<TextureID, std::string> m_texMap;
  std::unordered_map<FontID, std::string> m_fontMap;
  std::unordered_map<AnimID, std::string> m_animMap;

  std::string m_assetsPath = "assets.json";

  RenderWindow *m_renderWindow = nullptr;

  void parseTextures(const json &config);
  void parseFonts(const json &config);
  void parseAnims(const json &config);

public:
  AssetManager(RenderWindow *renderWindow) : m_renderWindow(renderWindow) {}

  void setAssetsPath(std::string path) { m_assetsPath = path; }
  bool parseManifest();

  SDL_Texture *idToTex(TextureID id);
  SDL_Texture *loadTex(std::string_view path);
  SDL_Texture *getTextTex(const std::string &text, FontID id, int size, SDL_Color color);
  TTF_Font *idToFont(FontID id, int size);
  const SpriteAnimation &idToAnim(AnimID id);
};
} // namespace vl
