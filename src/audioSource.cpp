#include "velox/components/audioSource.h"

#include "velox/assetManager.h"
#include "velox/audioMixer.h"
#include "velox/registry.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <cmath>

namespace vl {

void playSound(Registry &reg, AudioMixer &mixer, AudioSource &audio,
                       bool oneShot) {
  AssetManager &assets = reg.getResource<AssetManager>();
  MIX_Track *track = mixer.getFreeTrack();
  MIX_Audio *sound = assets.idToAudio(audio.id, mixer.getMixer());

  MIX_SetTrackGain(track, powf(10.0f, audio.dbOffset / 20.0f));
  MIX_SetTrackFrequencyRatio(track, audio.speed);

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
  SDL_DestroyProperties(options);
}

} // namespace vl
