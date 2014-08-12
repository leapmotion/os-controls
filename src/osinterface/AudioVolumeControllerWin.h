#pragma once
#include "AudioVolumeController.h"

struct IAudioEndpointVolume;
struct IAudioSessionManager;
struct IMMDevice;
struct IMMDeviceEnumerator;
struct ISimpleAudioVolume;

class AudioVolumeControllerWin:
  public AudioVolumeController
{
public:
  AudioVolumeControllerWin(void);

private:
  CComPtr<IMMDeviceEnumerator> m_devEnumerator;
  CComPtr<IMMDevice> m_pEndpoint;
  CComPtr<IAudioEndpointVolume> m_pAudioEndpointVolume;

public:
  // AudioVolumeController overrides:
  float GetVolume(void) override;
  void SetVolume(float volume) override;
  void SetMute(bool mute) override;
  bool IsMuted(void) override;
};
