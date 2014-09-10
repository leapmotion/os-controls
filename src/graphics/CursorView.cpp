#include "stdafx.h"
#include "CursorView.h"
#include "GLShaderLoader.h"
#include "RenderEngine.h"
#include "RenderFrame.h"
#include "RenderState.h"
#include "HandCursor.h"

#include "GLShader.h"
#include "GLShaderLoader.h"
#include "GLTexture2.h"
#include "TextFile.h"
#include "Resource.h"
#include <memory>
#include <iostream>

CursorView::CursorView() :
  Renderable{OSVector2(400, 400)},
  m_state(State::INACTIVE),
  m_alphaMask(0.0f, 0.2, EasingFunctions::QuadInOut<float>),
  m_scrollBody(new SVGPrimitive()),
  m_scrollLine(new SVGPrimitive()),
  m_scrollFingerLeft(new SVGPrimitive()),
  m_scrollFingerRight(new SVGPrimitive())
{
  Resource<TextFile> scrollBodyFile("scroll-cursor-body.svg");
  Resource<TextFile> scrollLineFile("scroll-cursor-line.svg");
  Resource<TextFile> scrollFingerFile("scroll-cursor-finger.svg");
  
  m_scrollBody->Set(scrollBodyFile->Contents());
  m_scrollLine->Set(scrollLineFile->Contents());
  m_scrollFingerLeft->Set(scrollFingerFile->Contents());
  m_scrollFingerRight->Set(scrollFingerFile->Contents());
  
  m_scrollBodyOffset = m_scrollBody->Origin() - (m_scrollBody->Size()/2.0);
  m_scrollLineOffset = m_scrollLine->Origin() - (m_scrollLine->Size()/2.0);
  m_scrollFingerLeftOffset = m_scrollFingerLeft->Origin() - (m_scrollFingerLeft->Size()/2.0);
  m_scrollFingerRightOffset = m_scrollFingerRight->Origin() - (m_scrollFingerRight->Size()/2.0);
}

CursorView::~CursorView() {
}

void CursorView::SetSize(float radius) {
}

void CursorView::AutoInit() {
  m_renderEngine->Add(shared_from_this());
}

void CursorView::AutoFilter(const Leap::Hand& hand, OSCState appState, const HandData& handData) {
  static const float PINCH_MIN = 0.0f;
  static const float PINCH_MAX = 0.65f;
  static const float FINGER_SPREAD_MIN = 23.0f;
  static const float FINGER_SPREAD_MAX = 50.0f;
  
  float pinchNorm = 0.0f;
  
  m_renderEngine->BringToFront(this);

  //State Transitions
  switch(m_state) {
    case State::INACTIVE:
      if(appState != OSCState::FINAL &&
         handData.handPose != HandPose::Clawed) {
        m_state = State::ACTIVE;
        m_alphaMask.Set(1.0f);
      }
      break;
    case State::ACTIVE:
      if(appState == OSCState::FINAL ||
         handData.handPose == HandPose::Clawed) {
        m_state = State::INACTIVE;
        m_alphaMask.Set(0.0f);
      }
      break;
  }
  
  //State Loops
  switch(m_state) {
    case State::ACTIVE:
      pinchNorm = (handData.pinchData.pinchStrength - PINCH_MIN) / (PINCH_MAX - PINCH_MIN);
      pinchNorm =  1.0f - std::min(1.0f, std::max(0.0f, pinchNorm));
      m_fingerSpread = FINGER_SPREAD_MIN + (pinchNorm * (FINGER_SPREAD_MAX - FINGER_SPREAD_MIN));
      position = OSVector2{handData.locationData.x, handData.locationData.y};
      break;
    case State::INACTIVE:
    default:
      break;
  }
}

void CursorView::AnimationUpdate(const RenderFrame &frame) {
  m_scrollBody->Translation() = Vector3(m_scrollBodyOffset.x(), m_scrollBodyOffset.y(), 0);
  m_scrollLine->Translation() = Vector3(m_scrollLineOffset.x(), m_scrollLineOffset.y(), 0);
  m_scrollFingerLeft->Translation() = Vector3(m_scrollFingerLeftOffset.x() - m_fingerSpread, m_scrollFingerLeftOffset.y(), 0);
  m_scrollFingerRight->Translation() = Vector3(m_scrollFingerRightOffset.x() + m_fingerSpread, m_scrollFingerRightOffset.y(), 0);
  m_alphaMask.Update(frame.deltaT.count());
}

void CursorView::Render(const RenderFrame& frame) const {
  switch ( m_state ) {
    case State::ACTIVE:
      PrimitiveBase::DrawSceneGraph(*m_scrollLine, frame.renderState);
      PrimitiveBase::DrawSceneGraph(*m_scrollBody, frame.renderState);
      PrimitiveBase::DrawSceneGraph(*m_scrollFingerLeft, frame.renderState);
      PrimitiveBase::DrawSceneGraph(*m_scrollFingerRight, frame.renderState);
      break;
    case State::INACTIVE:
    default:
      break;
  }
}
