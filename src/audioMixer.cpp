#include "velox/audioMixer.h"

namespace vl {

bool AudioMixer::initAudio() {
  if (!MIX_Init()) {
    SDL_Log("Couldn't init SDL_mixer: %s", SDL_GetError());
    return false;
  }

  m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
  if (!m_mixer) {
    SDL_Log("Couldn't create mixer: %s", SDL_GetError());
    return false;
  }

  m_trackPool.reserve(defaultTrackPoolSize);
  for (size_t i = 0; i < defaultTrackPoolSize; i++) {
    MIX_Track *raw = MIX_CreateTrack(m_mixer);
    if (raw) {
      m_trackPool.emplace_back(raw, trackDeleter);
    }
  }
  return true;
}

MIX_Track *AudioMixer::getFreeTrack() {
  if (!m_mixer) {
    SDL_Log("[Audio Mixer] getFreeTrack called before initAudio()");
    return nullptr;
  }
  for (auto &track : m_trackPool) {
    if (!MIX_TrackPlaying(track.get()))
      return track.get();
  }
  MIX_Track *raw = MIX_CreateTrack(m_mixer);
  if (!raw) {
    SDL_Log("Couldn't create track: %s", SDL_GetError());
    return nullptr;
  }

  m_trackPool.emplace_back(raw, trackDeleter);
  return m_trackPool.back().get();
}
} // namespace vl
