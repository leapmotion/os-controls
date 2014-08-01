#pragma once
#include "MediaController.h"

class MediaControllerMac:
  public MediaController
{
public:
  void PlayPause(void) override;
  void Stop(void) override;
  void Next(void) override;
  void Prev(void) override;
  void VolumeUp(void) override;
  void VolumeDown(void) override;
  void Mute(void) override;
};

