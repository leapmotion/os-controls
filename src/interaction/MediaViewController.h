#pragma once
#include "uievents/MediaViewEventListener.h"
#include "uievents/SystemMultimediaEventListener.h"

class MediaView;
class AudioVolumeInterface;
class MediaInterface;

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

  Autowired<AudioVolumeInterface> m_audioVolumeInterface;
  Autowired<MediaInterface> m_mediaInterface;

  //Implementation of MeidaViewEventListener
  void OnUserPlayPause() override;
  void OnUserNextTrack() override;
  void OnUserPrevTrack() override;
  void OnInitializeVolume() override;
  void OnUserChangedVolume(float dVolume) override;
  
  //Implementation of SystemMultimediaEventListener
  void OnVolumeChanged(float oldVolume, float newVolume) override;
  
private:
  // Need to store current volume as a float, since system volume
  // is stored with discrete steps on some systems, so we can't
  // rely on that as a model for incremental updating.
  float m_volume;
};

