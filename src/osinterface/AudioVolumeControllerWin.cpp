#include "stdafx.h"
#include "AudioVolumeControllerWin.h"
#include <Mmdeviceapi.h>

AudioVolumeControllerWin::AudioVolumeControllerWin(void) {
  // Enumerate all volume devices, find one we can talk to
  CComPtr<IMMDeviceEnumerator> devEnumerator;
}

AudioVolumeController* AudioVolumeController::New(void) {
  return new AudioVolumeControllerWin;
}

double AudioVolumeControllerWin::GetVolume(void) {
  return 0.0;
}
