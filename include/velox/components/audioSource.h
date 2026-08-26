#pragma once

#include "../resourceIDs.h"

namespace vl {

class Registry;
class AudioMixer;

struct AudioSource {
  AudioID id;
  float dbOffset = 0;
  float speed = 1.0f;
  bool loop;
};

void playSound(Registry &reg, AudioMixer &mixer, AudioSource &audio,
               bool oneShot = false);

}; // namespace vl
