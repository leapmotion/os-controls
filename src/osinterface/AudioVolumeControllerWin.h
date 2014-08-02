#pragma once
#include "AudioVolumeController.h"

struct IMMDevice;

class AudioVolumeControllerWin:
  public AudioVolumeController
{
public:
  AudioVolumeControllerWin(void);

private:
  CComPtr<IMMDevice> m_pDevice;

public:
  // AudioVolumeController overrides:
  double GetVolume(void) override;
};