#include "stdafx.h"
#include "MediaView.h"
#include "RenderState.h"

#include <iostream>

const static float PI = 3.14159265f;

MediaView::MediaView(const Vector3& center, float offset) :
m_time(0),
m_scale(1)
{
  m_leftButton = RenderEngineNode::Create<RadialButton>(50 - offset, 100, 3 * PI / 4, 5 * PI / 4, Vector3(-1 * offset, 0, 0));
  m_topButton = RenderEngineNode::Create<RadialButton>(50 - offset, 100, 5 * PI / 4, 7 * PI / 4, Vector3(0, -1 * offset, 0));
  m_rightButton = RenderEngineNode::Create<RadialButton>(50 - offset, 100, -PI / 4, PI / 4, Vector3(offset, 0, 0));
  m_volumeControl = RenderEngineNode::Create<VolumeControl>();

  Translation() = center;
}

void MediaView::InitChildren() {
  AddChild(m_leftButton);
  AddChild(m_topButton);
  AddChild(m_rightButton);
  AddChild(m_volumeControl);
}

void MediaView::AnimationUpdate(const RenderFrame& frame) {
  m_time += static_cast<float>(frame.deltaT.count());
  
  m_leftButton->nudge(20 * (0.5f + 0.5f * std::sin(5*m_time)));
  m_topButton->nudge(20 * (0.5f + 0.5f * std::sin(5*m_time + 2)));
  m_rightButton->nudge(20 * (0.5f + 0.5f * std::sin(5*m_time + 4)));

  //TODO: make this animate
  switch (m_fadeState) {
  case FADE_IN:
    setOpacity(1.0f);
    break;
  case FADE_OUT:
    setOpacity(0.0f);
    break;
  }

  frame.renderState.GetModelView().Scale(Vector3::Constant(m_scale + .5f*std::sin(3*m_time)));
}

void MediaView::SetFadeState(FadeState newState){
  m_fadeState = newState;
}

void MediaView::setOpacity(float opacity) {
  m_leftButton->setOpacity(opacity);
  m_topButton->setOpacity(opacity);
  m_rightButton->setOpacity(opacity);
}
