#pragma once

class AudioVolumeInterface;
class MediaView;

/// <summary>
/// Handles callbacks from the MediaView, and updates the view with information from the system
/// </summary>
class MediaViewController:
  public MediaViewEventListener,
  public Updatable
{
public:
  MediaViewController(void);
  ~MediaViewController(void);

  Autowired<AudioVolumeInterface> m_avi;
  Autowired<MediaView> m_mv;

  void Update(double deltaT) override;
  void OnVolumeChanged(float volume) override;
};

