#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_audio.h>
#include <SDL3_mixer/SDL_mixer.h>

#include <memory>
#include <vector>

namespace vl {

inline auto trackDeleter = [](MIX_Track *track) {
  if (track)
    MIX_DestroyTrack(track);
};

class AudioMixer {
private:
  std::vector<std::unique_ptr<MIX_Track, decltype(trackDeleter)>> m_trackPool;
  static constexpr size_t defaultTrackPoolSize = 20;

  MIX_Mixer *m_mixer = nullptr;

public:
  bool initAudio();

  MIX_Track *getFreeTrack();
  MIX_Mixer *getMixer() { return m_mixer; }
};
} // namespace vl
