#pragma once
#include "AudioVolumeController.h"

#include <CoreAudio/AudioHardware.h>

class AudioVolumeControllerMac:
  public AudioVolumeController
{
public:
  AudioVolumeControllerMac(void);

private:
  static AudioDeviceID GetAudioDeviceID();

public:
  // AudioVolumeController overrides:
  float GetVolume(void) override;
  void SetVolume(float volume) override;
};
