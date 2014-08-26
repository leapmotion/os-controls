#pragma once
#include "uievents/Updatable.h"

class AudioVolumeInterface;
class SystemMultimediaEventListener;

/// <summary>
/// Checks the system volume levels for changes, raises SystemMultimediaEventListener::OnVolumeChanged
/// </summary>
class VolumeLevelChecker:
  public Updatable
{
private:
  Autowired<AudioVolumeInterface> m_avi;
  AutoFired<SystemMultimediaEventListener> m_smel;

  // Volume as of the last check
  float m_oldVolume;

public:
  // Updatable overrides:
  void Tick(std::chrono::duration<double> deltaT) override;
};

