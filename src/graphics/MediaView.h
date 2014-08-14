#pragma once
#include "State.h"
#include "graphics/RadialButton.h"
#include "graphics/RenderFrame.h"
#include "graphics/VolumeControl.h"


#include "Primitives.h"


class MediaView {
public:
  MediaView(const Vector3& center, float offset);
  
  void AutoFilter(const RenderFrame& frame, const StateSentry<State::MediaView>& in);
  
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
  RadialButton m_leftButton;
  RadialButton m_topButton;
  RadialButton m_rightButton;
  
  float m_time;
  
  VolumeControl m_volumeControl;
};
