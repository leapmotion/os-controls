#pragma once
#include "State.h"
#include "graphics/RadialButton.h"
#include "graphics/RenderFrame.h"
#include "graphics/VolumeControl.h"
#include "graphics/MostRecent.h"
#include "interaction/HandExistTrigger.h"
#include "Leap.h"


#include "Primitives.h"


class MediaView {
public:
  MediaView(const Vector3& center, float offset);
  
  void AutoFilter(const RenderFrame& frame, const StateSentry<State::MediaView>& in);
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  enum FadeState {
    FADE_OUT,
    FADE_IN
  };
  
  FadeState fadeState;
  
  AutoRequired<MostRecent> m_hes;
  
  RadialButton m_leftButton;
  RadialButton m_topButton;
  RadialButton m_rightButton;
  
  float m_time;
  Leap::Hand controllingHand;
  
  VolumeControl m_volumeControl;
  
  bool findHandInVector(std::vector<Leap::Hand> vect, Leap::Hand goalHand);
};
