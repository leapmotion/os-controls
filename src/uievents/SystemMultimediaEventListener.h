#pragma once

/// <summary>
/// An event which represents a multimedia event originating with the operating system
/// </summary>
class SystemMultimediaEventListener {
public:
  /// <summary>
  /// Indicates that a change to the mute status has taken place
  /// </summary>
  /// <param name="muted">True if the system is presently muted</param>
  virtual void OnMuteChanged(bool muted) {}

  /// <summary>
  /// Indicates that the volume has changed from a previously recorded value
  /// </summary>
  virtual void OnVolumeChanged(float oldVolume, float newVolume) {}
};