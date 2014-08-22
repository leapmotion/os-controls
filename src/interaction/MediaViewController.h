#pragma once
#include "graphics/MediaView.h"

/// <summary>
/// Handles callbacks from the MediaView, and updates the view with information from the system
/// </summary>
class MediaViewController:
  public MediaViewEventListener
{
public:
  MediaViewController(void);
  ~MediaViewController(void);

  AutoRequired<MediaView> mediaView;

  virtual void OnVolumeChanged(float volume) override {
    ;
  };
};

