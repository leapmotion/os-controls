#pragma once
#include "uievents/MediaViewEventListener.h"
#include "uievents/SystemMultimediaEventListener.h"

class AbstractVolumeControl;
class AudioVolumeInterface;

/// <summary>
/// Handles callbacks from the MediaView, and updates the view with information from the system
/// </summary>
class MediaViewController:
  public MediaViewEventListener,
  public SystemMultimediaEventListener
{
public:
  MediaViewController(void);
  ~MediaViewController(void);

  Autowired<AudioVolumeInterface> m_avi;
  Autowired<AbstractVolumeControl> m_mv;

  //Implementation of MeidaViewEventListener
  void OnUserPlayPause() override;
  void OnUserNextTrack() override;
  void OnUserPrevTrack() override;
  void OnUserChangedVolume(float volume) override;
  
  //Implementation of SystemMultimediaEventListener
  void OnVolumeChanged(float oldVolume, float newVolume) override;
};

