#include "stdafx.h"
#include "CursorView.h"
#include "GLShaderLoader.h"
#include "RenderEngine.h"
#include "RenderFrame.h"
#include "RenderState.h"
#include "HandCursor.h"
#include "interaction/interactionConfigs.h"

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
  m_scrollBody(new SVGPrimitive()),
  m_scrollLine(new SVGPrimitive()),
  m_scrollFingerLeft(new SVGPrimitive()),
  m_scrollFingerRight(new SVGPrimitive()),
  m_disk(new Disk()),
  m_fingerSpread(0.0f),
  m_pinchStrength(0.0f),
  m_wasPinching(false),
  m_lastHandPosition(0,0),
  m_isPointing(false)
{
  const Color CURSOR_COLOR(0.505f, 0.831f, 0.114f, 0.95f);
  
  // Initialize Smoothed member variables.
  m_bodyOffset.SetInitialValue(0.0f);
  m_bodyOffset.SetSmoothStrength(0.8f);
  
  m_x.SetSmoothStrength(0.6f);
  m_y.SetSmoothStrength(0.6f);
  
  m_bodyAlpha.SetSmoothStrength(0.3f);
  m_bodyAlpha.SetInitialValue(0.0f);
  
  m_diskAlpha.SetSmoothStrength(0.5f);
  m_diskAlpha.SetInitialValue(0.0f);
  
  
  //Initialize Disk Cursor
  m_disk->SetRadius(20.0f);
  m_disk->Material().SetAmbientLightColor(CURSOR_COLOR);
  m_disk->Material().SetDiffuseLightColor(CURSOR_COLOR);
  m_disk->Material().SetAmbientLightingProportion(1.0f);
  
  // Load SVG Files for scroll cursor parts
  Resource<TextFile> scrollBodyFile("scroll-cursor-body.svg");
  Resource<TextFile> scrollLineFile("scroll-cursor-line.svg");
  Resource<TextFile> scrollFingerRightFile("scroll-cursor-finger_right.svg");
  Resource<TextFile> scrollFingerLeftFile("scroll-cursor-finger_left.svg");
  
  // Assign SVG data to proper primitives.
  m_scrollBody->Set(scrollBodyFile->Contents());
  m_scrollLine->Set(scrollLineFile->Contents());
  m_scrollFingerLeft->Set(scrollFingerRightFile->Contents());
  m_scrollFingerRight->Set(scrollFingerLeftFile->Contents());
  
  // Calulate the offsets to the svg primitive centers.
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

void CursorView::AutoFilter(const Leap::Hand& hand, OSCState appState, const HandData& handData, const FrameTime& frameTime) {
  static const float FINGER_SPREAD_MIN = 23.0f;
  static const float FINGER_SPREAD_MAX = 100.0f;
  static const float SCROLL_VELOCITY_MIN = 0.0f;
  static const float SCROLL_VELOCITY_MAX = 20.0f;
  static const float BODY_OFFSET_MAX = 100.0f;
  
  int velocitySign = 1;
  float velocityNorm = 0.0f;
  float goalBodyOffset = 0.0f;
  
  m_renderEngine->BringToFront(this);

  //State Transitions
  switch(m_state) {
    case State::INACTIVE:
      if(appState != OSCState::FINAL ) {
        m_state = State::ACTIVE;
      }
      break;
    case State::ACTIVE:
      if ( handData.handPose == HandPose::OneFinger ) {
        m_isPointing = true;
      }
      else {
        m_isPointing = false;
      }
      
      if(appState == OSCState::FINAL) {
        m_state = State::INACTIVE;
      }
      break;
  }
  
  //State Loops
  switch(m_state) {
    case State::ACTIVE:
    {
      m_pinchStrength = handData.pinchData.pinchStrength;
      float spreadNorm = m_pinchStrength / activationConfigs::MIN_PINCH_CONTINUE;
      spreadNorm = std::max(0.0f, std::min(1.0f, spreadNorm));
      m_fingerSpread = FINGER_SPREAD_MIN + ((1-spreadNorm) * (FINGER_SPREAD_MAX - FINGER_SPREAD_MIN));
    
      if ( handData.pinchData.isPinching ) {
        velocitySign = handData.locationData.dY < 0 ? -1 : 1;
        velocityNorm = (fabs(handData.locationData.dY) - SCROLL_VELOCITY_MIN) / (SCROLL_VELOCITY_MAX - SCROLL_VELOCITY_MIN);
        velocityNorm = std::min(1.0f, std::max(0.0f, velocityNorm));
        velocityNorm *= velocitySign;
        goalBodyOffset = velocityNorm * BODY_OFFSET_MAX;
        m_bodyOffset.SetGoal(goalBodyOffset);
      }
      else {
        m_x.SetGoal(handData.locationData.x);
        m_y.SetGoal(handData.locationData.y);
        if ( m_bodyOffset.Goal() != 0.0f ) { m_bodyOffset.SetGoal(0.0f); }
      }
      
      m_wasPinching = handData.pinchData.isPinching;
    
      m_lastHandPosition = handData.locationData.screenPosition();
      break;
    }
    case State::INACTIVE:
      m_bodyOffset.SetInitialValue(0.0f);
    default:
      break;
  }
}

void CursorView::AnimationUpdate(const RenderFrame &frame) {
  const float MIN_PINCH_NORM = 0.5f;
  
  float fingerOpacity = (m_pinchStrength - MIN_PINCH_NORM) / (activationConfigs::MIN_PINCH_CONTINUE - MIN_PINCH_NORM);
  m_scrollFingerLeft->LocalProperties().AlphaMask() = fingerOpacity;
  m_scrollFingerRight->LocalProperties().AlphaMask() = fingerOpacity;
  
  float bodyOpacityNorm = 0.0f;
  
  if ( m_wasPinching ) {
    bodyOpacityNorm = (m_pinchStrength - activationConfigs::MIN_PINCH_CONTINUE) / (activationConfigs::MIN_PINCH_START - activationConfigs::MIN_PINCH_CONTINUE);
  }
  else {
    bodyOpacityNorm = (m_pinchStrength - MIN_PINCH_NORM) / (activationConfigs::MIN_PINCH_START - MIN_PINCH_NORM);
  }
  
  bodyOpacityNorm = std::max(0.0f, std::min(1.0f, bodyOpacityNorm));
  m_bodyAlpha.SetGoal(bodyOpacityNorm);
  
  if ( m_isPointing ) {
    m_bodyAlpha.SetGoal(0.0f);
  }
  
  m_bodyAlpha.Update(frame.deltaT.count());
  
  if ( m_state == State::ACTIVE ) {
    m_x.Update(frame.deltaT.count());
    m_y.Update(frame.deltaT.count());
    m_ghostX.Update(frame.deltaT.count());
    m_ghostY.Update(frame.deltaT.count());
    m_bodyOffset.Update(frame.deltaT.count());
    position = OSVector2{m_ghostX, m_ghostY};
  }
  
  m_scrollBody->LocalProperties().AlphaMask() = m_bodyAlpha.Value();
  m_scrollLine->LocalProperties().AlphaMask() = m_bodyAlpha.Value();
  
  if ( m_bodyAlpha.Value() < 0.2f ) {
    m_diskAlpha.SetGoal(1.0f);
  }
  else {
    m_diskAlpha.SetGoal(0.0f);
  }
  
  m_diskAlpha.Update(frame.deltaT.count());
  
  m_disk->LocalProperties().AlphaMask() = m_diskAlpha;
  
  
  // Snapped Scrolling Cursor Positioning
  m_scrollBody->Translation() = Vector3(m_scrollBodyOffset.x() - position.x + m_x.Value(), m_scrollBodyOffset.y() - position.y + m_bodyOffset + m_y.Value(), 0.0f);
  m_scrollLine->Translation() = Vector3(m_scrollLineOffset.x() - position.x + m_x.Value(), m_scrollLineOffset.y() - position.y + m_y.Value(), 0.0f);
  m_scrollFingerLeft->Translation() = Vector3(m_scrollFingerLeftOffset.x() - position.x + m_fingerSpread + m_x.Value(), m_scrollFingerLeftOffset.y() - position.y + m_bodyOffset + m_y.Value(), 0.0f);
  m_scrollFingerRight->Translation() = Vector3(m_scrollFingerRightOffset.x() - position.x - m_fingerSpread + m_x.Value(), m_scrollFingerRightOffset.y() - position.y + m_bodyOffset + m_y.Value(), 0.0f);
  
  //Disk that appears when you're not doing things with scroll
  m_disk->Translation() = Vector3(-position.x + m_x, -position.y + m_y, 0.0f);
}

void CursorView::Render(const RenderFrame& frame) const {
  switch ( m_state ) {
    case State::ACTIVE:
      PrimitiveBase::DrawSceneGraph(*m_scrollLine, frame.renderState);
      PrimitiveBase::DrawSceneGraph(*m_scrollBody, frame.renderState);
      PrimitiveBase::DrawSceneGraph(*m_scrollFingerLeft, frame.renderState);
      PrimitiveBase::DrawSceneGraph(*m_scrollFingerRight, frame.renderState);
      PrimitiveBase::DrawSceneGraph(*m_disk, frame.renderState);
      break;
    case State::INACTIVE:
    default:
      break;
  }
}

Vector2 CursorView::getWindowCenter(OSWindow& window) {
  Vector2 position(window.GetPosition().x, window.GetPosition().y);
  Vector2 halfSize(window.GetSize().width / 2.0f, window.GetSize().height / 2.0f);
  return position + (halfSize);
}
