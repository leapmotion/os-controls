#pragma once

#include "graphics/RadialButton.h"
#include "uievents/MediaViewEventListener.h"

class Wedge :
  public RadialButton
{
  using RadialButton::RadialButton;
public:
  virtual void OnSelected() {};
protected:
  // Events fired by this Wedge
  AutoFired<MediaViewEventListener> m_mediaViewEventListener;
};

class PlayPauseWedge :
  public Wedge
{
  using Wedge::Wedge;
public:
  void OnSelected() override {
    m_mediaViewEventListener(&MediaViewEventListener::OnUserPlayPause);
  }
};

class PrevWedge :
public Wedge
{
  using Wedge::Wedge;
public:
  void OnSelected() override {
    m_mediaViewEventListener(&MediaViewEventListener::OnUserNextTrack);
  }
};

class NextWedge :
public Wedge
{
  using Wedge::Wedge;
public:
  void OnSelected() override {
    m_mediaViewEventListener(&MediaViewEventListener::OnUserPrevTrack);
  }
};