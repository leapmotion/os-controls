#pragma once

/// <summary>
/// Possible events that might happen while the media view is being drawn
/// </summary>
class MediaViewEventListener {
public:
  virtual void OnPlay(void) {}
  virtual void OnPause(void) {}
  virtual void OnVolumeChanged(float volume) {}
};
