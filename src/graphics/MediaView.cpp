#include "stdafx.h"
#include "MediaView.h"
#include "RenderState.h"

#include <iostream>

const static float PI = 3.14159265f;

MediaView::MediaView(const Vector3& center, float offset) :
  m_leftButton(center, 50-offset, 100, 3*PI/4, 5*PI/4, Vector3(-1 * offset, 0, 0)),
  m_topButton(center, 50-offset, 100, 5*PI/4, 7*PI/4, Vector3(0, -1 * offset, 0)),
  m_rightButton(center, 50-offset, 100, -PI/4, PI/4, Vector3(offset, 0, 0))
{
}

void MediaView::AutoFilter(const RenderFrame& frame, const StateSentry<State::MediaView>& in) {
  m_time += frame.deltaT.count();
  
  m_leftButton.nudge(20 * (0.5 + 0.5 * std::sin(5*m_time)));
  m_topButton.nudge(20 * (0.5 + 0.5 * std::sin(5*m_time + 2)));
  m_rightButton.nudge(20 * (0.5 + 0.5 * std::sin(5*m_time + 4)));
  
  m_leftButton.draw(frame);
  m_topButton.draw(frame);
  m_rightButton.draw(frame);
}