#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>
#include <SDL3_mixer/SDL_mixer.h>

#include "../resourceIDs.h"
#include "velox/assetManager.h"
#include "velox/registry.h"

namespace vl {

struct AudioSource {
  AudioID id;
  bool loop;
};

inline void playSound(Registry &reg, AudioSource &audio, bool oneShot = false) {
  AssetManager &assets = reg.getResource<AssetManager>();
  MIX_Track *track = assets.getFreeTrack();
  MIX_Audio *sound = assets.idToAudio(audio.id);

  MIX_SetTrackAudio(track, sound);
  if (!audio.loop || oneShot) {
    MIX_PlayTrack(track, 0);
    return;
  }

  SDL_PropertiesID options = SDL_CreateProperties();
  if (!options) {
    SDL_Log("[Audio Source: %s] Couldn't create play options: %s",
            audio.id.c_str(), SDL_GetError());
    return;
  }
  SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
  MIX_PlayTrack(track, options);
}

}; // namespace vl
