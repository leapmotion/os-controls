#include "stdafx.h"
#include "AudioVolumeControllerMac.h"

AudioVolumeControllerMac::AudioVolumeControllerMac(void) {
}

AudioVolumeController* AudioVolumeController::New(void) {
  return new AudioVolumeControllerMac;
}

float AudioVolumeControllerMac::GetVolume(void) {
  float level = 0;
  return level;
}

void AudioVolumeControllerMac::SetVolume(float volume) {
}
