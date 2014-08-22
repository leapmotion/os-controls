#pragma once

class AbstractVolumeControl {
public:
  virtual float Volume(void) = 0;
  virtual void SetVolume(float volume) = 0;
  virtual void NudgeVolume(float dVolume) = 0;
};