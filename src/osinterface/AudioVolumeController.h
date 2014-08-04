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
  virtual float GetVolume(void) = 0;

  /// <summary>
  /// Mutator counterpart to GetVolume
  /// </returns>
  virtual void SetVolume(float volume) = 0;
};