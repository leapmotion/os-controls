#include "stdafx.h"
#include "AudioVolumeControllerMac.h"

AudioVolumeControllerMac::AudioVolumeControllerMac(void) {
}

AudioVolumeController* AudioVolumeController::New(void) {
  return new AudioVolumeControllerMac;
}

float AudioVolumeControllerMac::GetVolume(void) {
  float volume = 0;
  AudioDeviceID audioDevice = GetAudioDeviceID();
  // Try to get the master channel. If it fails, try to then get left channel and right channel
  for (AudioObjectPropertyElement channel = 0; channel < 3; channel++) {
    AudioObjectPropertyAddress propertyAddress = { kAudioDevicePropertyVolumeScalar,
                                                   kAudioDevicePropertyScopeOutput,
                                                   channel };
    Float32 theVolume = 0;
    if (AudioObjectHasProperty(audioDevice, &propertyAddress)) {
      UInt32 size = sizeof(Float32);
      AudioObjectGetPropertyData(audioDevice, &propertyAddress, 0, nullptr, &size, &theVolume);
      float channelVolume = static_cast<float>(theVolume);
      if (channelVolume > volume) {
        volume = channelVolume;
      }
      if (channel == 0) { // If master channel, nothing left to do
        break;
      }
    }
  }
  return volume;
}

void AudioVolumeControllerMac::SetVolume(float volume) {
  AudioDeviceID audioDevice = GetAudioDeviceID();
  // Try to set the master channel. If it fails, try to then set left channel and right channel
  for (AudioObjectPropertyElement channel = 0; channel < 3; channel++) {
    AudioObjectPropertyAddress propertyAddress = { kAudioDevicePropertyVolumeScalar,
                                                   kAudioDevicePropertyScopeOutput,
                                                   channel };
    if (AudioObjectHasProperty(audioDevice, &propertyAddress)) {
      Float32 theVolume = static_cast<Float32>(volume);
      UInt32 size = sizeof(Float32);
      AudioObjectSetPropertyData(audioDevice, &propertyAddress, 0, nullptr, size, &theVolume);
      if (channel == 0) { // If master channel, nothing left to do
        break;
      }
    }
  }
}

AudioDeviceID AudioVolumeControllerMac::GetAudioDeviceID() {
  AudioObjectPropertyAddress outputDeviceAddress = { kAudioHardwarePropertyDefaultOutputDevice,
                                                     kAudioObjectPropertyScopeGlobal,
                                                     kAudioObjectPropertyElementMaster };
  AudioDeviceID outputDeviceID = 0;
  UInt32 size = sizeof(AudioDeviceID);
  AudioObjectGetPropertyData(kAudioObjectSystemObject, &outputDeviceAddress, 0, nullptr, &size, &outputDeviceID);
  return outputDeviceID;
}
