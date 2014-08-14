#include "stdafx.h"
#include "MediaView.h"
#include "RenderState.h"

#include <iostream>

const static float PI = 3.14159265f;

MediaView::MediaView(const Vector3& center, float offset) :
  m_leftButton(new RadialButton(center, 50 - offset, 100, 3 * PI / 4, 5 * PI / 4, Vector3(-1 * offset, 0, 0))),
  m_topButton(new RadialButton(center, 50-offset, 100, 5*PI/4, 7*PI/4, Vector3(0, -1 * offset, 0))),
  m_rightButton(new RadialButton(center, 50 - offset, 100, -PI / 4, PI / 4, Vector3(offset, 0, 0))),
  m_time(0)
{
}

void MediaView::InitChildren() {
  AddChild(m_leftButton);
  AddChild(m_topButton);
  AddChild(m_rightButton);
}

void MediaView::AnimationUpdate(const RenderFrame& frame) {
  m_time += static_cast<float>(frame.deltaT.count());
  
  m_leftButton->nudge(20 * (0.5f + 0.5f * std::sin(5*m_time)));
  m_topButton->nudge(20 * (0.5f + 0.5f * std::sin(5*m_time + 2)));
  m_rightButton->nudge(20 * (0.5f + 0.5f * std::sin(5*m_time + 4)));
}