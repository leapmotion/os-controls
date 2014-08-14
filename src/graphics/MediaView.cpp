#include "stdafx.h"
#include "MediaView.h"
#include "RenderState.h"

#include <iostream>

const static double PI = 3.14159265;

MediaView::MediaView(const Vector3& center, float offset) :
  m_leftButton(center, 50-offset, 100, 3*PI/4, 5*PI/4, Vector3(-1 * offset, 0, 0)),
  m_topButton(center, 50-offset, 100, 5*PI/4, 7*PI/4, Vector3(0, -1 * offset, 0)),
  m_rightButton(center, 50-offset, 100, -PI/4, PI/4, Vector3(offset, 0, 0))
{
  controllingHand = Leap::Hand::invalid();
  fadeState = FADE_OUT;
}

void MediaView::AutoFilter(const RenderFrame& frame, const StateSentry<State::MediaView>& in) {
  m_time += frame.deltaT.count();
  
  if(!controllingHand.isValid()) { // if there is a controlling hand
//    std::cout << "no controlling hand" << std::endl;
    if(!m_hes->m_mostRecent.m_newHands.empty())
    {
      controllingHand = m_hes->m_mostRecent.m_newHands[0];
      fadeState = FADE_IN;
      std::cout << "set new controlling hand" << std::endl;
    }
  }
  else {
    //std::cout << "has controlling hand" << std::endl;
    if(!findHandInVector(m_hes->m_mostRecent.m_stableHands, controllingHand)) {
      fadeState = FADE_OUT;
      controllingHand = Leap::Hand::invalid();
      std::cout << "remove controlling hand" << std::endl;
    }
  }
  
  m_leftButton.nudge(20 * (0.5 + 0.5 * std::sin(5*m_time)));
  m_topButton.nudge(20 * (0.5 + 0.5 * std::sin(5*m_time + 2)));
  m_rightButton.nudge(20 * (0.5 + 0.5 * std::sin(5*m_time + 4)));
  
  switch (fadeState) {
    case FADE_IN:
      m_leftButton.setOpacity(1.0f);
      m_topButton.setOpacity(1.0f);
      m_rightButton.setOpacity(1.0f);
      break;
    case FADE_OUT:
      m_leftButton.setOpacity(0.0f);
      m_topButton.setOpacity(0.0f);
      m_rightButton.setOpacity(0.0f);
      break;
  }
  m_leftButton.draw(frame);
  m_topButton.draw(frame);
  m_rightButton.draw(frame);
}

bool MediaView::findHandInVector(std::vector<Leap::Hand> vect, Leap::Hand goalHand) {
  bool retVal = false;
  for(Leap::Hand hand : vect) {
    if(hand.id() == goalHand.id()) {
      retVal = true;
    }
  }
  return retVal;
}

