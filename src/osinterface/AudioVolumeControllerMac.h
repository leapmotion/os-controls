#pragma once
#include "AudioVolumeController.h"

class AudioVolumeControllerMac:
  public AudioVolumeController
{
public:
  AudioVolumeControllerMac(void);

private:

public:
  // AudioVolumeController overrides:
  float GetVolume(void) override;
  void SetVolume(float volume) override;
};

