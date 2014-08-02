#pragma once

class AudioVolumeController:
  public ContextMember
{
public:
  AudioVolumeController(void);

  static AudioVolumeController* New(void);

private:

public:
  /// <returns>
  /// Volume on current primary audio device, in the range [0, 1], referring to the percentage of maximum value
  /// </returns>
  virtual double GetVolume(void) = 0;
};