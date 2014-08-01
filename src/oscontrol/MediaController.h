#pragma once

class MediaController:
  public ContextMember
{
public:
  MediaController(void);
  ~MediaController(void);

  static MediaController* New(void);

public:
  virtual void PlayPause(void) = 0;
  virtual void Stop(void) = 0;
  virtual void Next(void) = 0;
  virtual void Prev(void) = 0;
  virtual void VolumeUp(void) = 0;
  virtual void VolumeDown(void) = 0;
  virtual void Mute(void) = 0;
};

