#pragma once

#include "graphics/RadialButton.h"
#include "uievents/MediaViewEventListener.h"

class Wedge :
  public RadialButton
{
  //using RadialButton::RadialButton;
public:
  Wedge(float innerRadius, float width, float startAngle, float endAngle, const Vector3& offset, bool isNullWedge = false) :
  RadialButton(innerRadius, width, startAngle, endAngle, offset, isNullWedge) {};
  virtual void OnSelected() {};
protected:
  // Events fired by this Wedge
  AutoFired<MediaViewEventListener> m_mediaViewEventListener;
};

class PlayPauseWedge :
  public Wedge
{
  //using RadialButton::RadialButton;
public:
  PlayPauseWedge(float innerRadius, float width, float startAngle, float endAngle, const Vector3& offset, bool isNullWedge = false) :
  Wedge(innerRadius, width, startAngle, endAngle, offset, isNullWedge) {};
  void OnSelected() override {
    m_mediaViewEventListener(&MediaViewEventListener::OnUserPlayPause);
  }
};

class PrevWedge :
public Wedge
{
  //using RadialButton::RadialButton;
public:
  PrevWedge(float innerRadius, float width, float startAngle, float endAngle, const Vector3& offset, bool isNullWedge = false) :
  Wedge(innerRadius, width, startAngle, endAngle, offset, isNullWedge) {};
  
  void OnSelected() override {
    m_mediaViewEventListener(&MediaViewEventListener::OnUserNextTrack);
  }
};

class NextWedge :
public Wedge
{
  //using RadialButton::RadialButton;
public:
  NextWedge(float innerRadius, float width, float startAngle, float endAngle, const Vector3& offset, bool isNullWedge = false) :
  Wedge(innerRadius, width, startAngle, endAngle, offset, isNullWedge) {};
  
  void OnSelected() override {
    m_mediaViewEventListener(&MediaViewEventListener::OnUserPrevTrack);
  }
};