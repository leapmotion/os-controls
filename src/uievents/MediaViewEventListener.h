#pragma once

/// <summary>
/// An event type that is raised when an event happens originating with the user
/// </summary>
class MediaViewEventListener {
public:
  virtual void OnUserPlay(void) {}
  virtual void OnUserPause(void) {}
  virtual void OnUserChangedVolume(float volume) {}
};
