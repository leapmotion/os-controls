#include "stdafx.h"
#include "AudioVolumeControllerWin.h"
#include <Endpointvolume.h>
#include <Mmdeviceapi.h>

AudioVolumeControllerWin::AudioVolumeControllerWin(void) {
  // Enumerate all volume devices, find one we can talk to
  m_devEnumerator.CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL);
  if(!m_devEnumerator)
    throw std::runtime_error("Failed to create a multimedia device enumerator");

  HRESULT hr = m_devEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &m_pEndpoint);
  if(FAILED(hr))
    throw std::runtime_error("Failed to get a default audio endpoint");

  hr = m_pEndpoint->Activate(__uuidof(*m_pAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**) &m_pAudioEndpointVolume);
  if(FAILED(hr))
    throw std::runtime_error("Cannot open a handle to the audio volume session manager");
}

AudioVolumeController* AudioVolumeController::New(void) {
  return new AudioVolumeControllerWin;
}

float AudioVolumeControllerWin::GetVolume(void) {
  float level;
  m_pAudioEndpointVolume->GetMasterVolumeLevelScalar(&level);
  return level;
}

void AudioVolumeControllerWin::SetVolume(float volume) {
  m_pAudioEndpointVolume->SetMasterVolumeLevelScalar(volume, nullptr);
}