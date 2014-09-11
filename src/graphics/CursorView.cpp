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
  m_ghostCursor(new SVGPrimitive()),
  m_scrollBody(new SVGPrimitive()),
  m_scrollLine(new SVGPrimitive()),
  m_scrollFingerLeft(new SVGPrimitive()),
  m_scrollFingerRight(new SVGPrimitive()),
  m_disk(new Disk()),
  m_fingerSpread(0.0f),
  m_pinchNormal(0.0f),
  m_wasPinching(false),
  m_lastHandPosition(0,0),
  m_isPointing(false)
{
  m_bodyOffset.SetInitialValue(0.0f);
  m_bodyOffset.SetSmoothStrength(0.8f);
  
  m_x.SetSmoothStrength(0.6f);
  m_y.SetSmoothStrength(0.6f);
  
  m_ghostX.SetSmoothStrength(0.8f);
  m_ghostY.SetSmoothStrength(0.8f);
  
  m_bodyAlpha.SetSmoothStrength(0.3f);
  m_bodyAlpha.SetInitialValue(0.0f);
  
  m_disk->SetRadius(20.0f);
  
  Resource<TextFile> scrollBodyFile("scroll-cursor-body.svg");
  Resource<TextFile> scrollLineFile("scroll-cursor-line.svg");
  Resource<TextFile> scrollFingerFile("scroll-cursor-finger.svg");
  Resource<TextFile> ghostCursorFile("scroll-cursor-ghost.svg");
  
  m_ghostCursor->Set(ghostCursorFile->Contents());
  m_scrollBody->Set(scrollBodyFile->Contents());
  m_scrollLine->Set(scrollLineFile->Contents());
  m_scrollFingerLeft->Set(scrollFingerFile->Contents());
  m_scrollFingerRight->Set(scrollFingerFile->Contents());
  
  m_ghostCursorOffset = m_ghostCursor->Origin() - (m_ghostCursor->Size()/2.0);
  m_scrollBodyOffset = m_scrollBody->Origin() - (m_scrollBody->Size()/2.0);
  m_scrollLineOffset = m_scrollLine->Origin() - (m_scrollLine->Size()/2.0);
  m_scrollFingerLeftOffset = m_scrollFingerLeft->Origin() - (m_scrollFingerLeft->Size()/2.0);
  m_scrollFingerRightOffset = m_scrollFingerRight->Origin() - (m_scrollFingerRight->Size()/2.0);
  
  m_ghostCursor->LocalProperties().AlphaMask() = GHOST_OPACITY;
}

CursorView::~CursorView() {
}

void CursorView::SetSize(float radius) {
}

void CursorView::AutoInit() {
  m_renderEngine->Add(shared_from_this());
}

void CursorView::AutoFilter(const Leap::Hand& hand, OSCState appState, const HandData& handData, const FrameTime& frameTime) {
  static const float PINCH_MIN = 0.0f;
  static const float PINCH_MAX = 0.65f;
  static const float FINGER_SPREAD_MIN = 23.0f;
  static const float FINGER_SPREAD_MAX = 50.0f;
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
        m_alphaMask.Set(1.0f);
        
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
        m_alphaMask.Set(0.0f);
      }
      break;
  }
  
  //State Loops
  switch(m_state) {
    case State::ACTIVE:
      m_pinchNormal = (handData.pinchData.pinchStrength - PINCH_MIN) / (PINCH_MAX - PINCH_MIN);
      m_pinchNormal =  1.0f - std::min(1.0f, std::max(0.0f, m_pinchNormal));
      m_fingerSpread = FINGER_SPREAD_MIN + (m_pinchNormal * (FINGER_SPREAD_MAX - FINGER_SPREAD_MIN));
    
      if ( handData.pinchData.isPinching ) {
        
        if ( !m_wasPinching ) {
          if ( m_osWindowMonitor ) {
            std::shared_ptr<OSWindow> newWindow = m_osWindowMonitor->WindowFromPoint(OSPointMake(handData.locationData.x, handData.locationData.y));
            if( newWindow && newWindow != m_lastSelectedWindow ) {
              updateScrollerPosition();
            } else if ( !newWindow ) {
              m_ghostCursor->LocalProperties().AlphaMask() = 0.0f;
            }
            m_lastSelectedWindow = newWindow;
          }
        }
        
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
    case State::INACTIVE:
      m_bodyOffset.SetInitialValue(0.0f);
    default:
      break;
  }
}

void CursorView::AnimationUpdate(const RenderFrame &frame) {
  const float MIN_PINCH_START = 0.85f;
  const float MIN_PINCH_CONTINUE = 0.8f;
  const float MIN_PINCH_NORM = 0.5f;
  const float MAX_PINCH_NORM = 0.8f;
  
  float reversedPinchNorm = 1 - m_pinchNormal; //the pinch normal is mapped the opposite of what we want reverse it.
  float fingerOpacity = (reversedPinchNorm - MIN_PINCH_NORM) / (MAX_PINCH_NORM - MIN_PINCH_NORM);
  m_scrollFingerLeft->LocalProperties().AlphaMask() = fingerOpacity;
  m_scrollFingerRight->LocalProperties().AlphaMask() = fingerOpacity;
  
  float bodyOpacityNorm = 0.0f;
  
  if ( m_wasPinching ) {
    bodyOpacityNorm = (reversedPinchNorm - MIN_PINCH_CONTINUE) / (MIN_PINCH_START - MIN_PINCH_CONTINUE);
  }
  else {
    bodyOpacityNorm = (reversedPinchNorm - 0.4f) / (MIN_PINCH_START - 0.4f);
  }
  
  bodyOpacityNorm = std::max(0.0f, std::min(1.0f, bodyOpacityNorm));
  std::cout << "body opacity norm: " << bodyOpacityNorm << std::endl;
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
  
  m_disk->LocalProperties().AlphaMask() = 1 - m_bodyAlpha.Value();
  
  // Snapped Scrolling Cursor Positioning
  m_scrollBody->Translation() = Vector3(m_scrollBodyOffset.x() - position.x + m_x.Value(), m_scrollBodyOffset.y() - position.y + m_bodyOffset + m_y.Value(), 0.0f);
  m_scrollLine->Translation() = Vector3(m_scrollLineOffset.x() - position.x + m_x.Value(), m_scrollLineOffset.y() - position.y + m_y.Value(), 0.0f);
  m_scrollFingerLeft->Translation() = Vector3(m_scrollFingerLeftOffset.x() - position.x - m_fingerSpread + m_x.Value(), m_scrollFingerLeftOffset.y() - position.y + m_bodyOffset + m_y.Value(), 0.0f);
  m_scrollFingerRight->Translation() = Vector3(m_scrollFingerRightOffset.x() - position.x + m_fingerSpread + m_x.Value(), m_scrollFingerRightOffset.y() - position.y + m_bodyOffset + m_y.Value(), 0.0f);
  
  //Disk that appears when you're not doing things with scroll
  m_disk->Translation() = Vector3(-position.x + m_x, -position.y + m_y, 0.0f);
  
  // Ghost Guide Cursor positioning
  m_ghostCursor->Translation() = Vector3(m_ghostCursorOffset.x(), m_ghostCursorOffset.y(), 0.0f);
  
  m_alphaMask.Update(frame.deltaT.count());
}

void CursorView::Render(const RenderFrame& frame) const {
  switch ( m_state ) {
    case State::ACTIVE:
      PrimitiveBase::DrawSceneGraph(*m_disk, frame.renderState);
      PrimitiveBase::DrawSceneGraph(*m_ghostCursor, frame.renderState);
      if ( m_lastSelectedWindow ) {
        PrimitiveBase::DrawSceneGraph(*m_scrollLine, frame.renderState);
        PrimitiveBase::DrawSceneGraph(*m_scrollBody, frame.renderState);
        PrimitiveBase::DrawSceneGraph(*m_scrollFingerLeft, frame.renderState);
        PrimitiveBase::DrawSceneGraph(*m_scrollFingerRight, frame.renderState);
      }
      break;
    case State::INACTIVE:
    default:
      break;
  }
}

void CursorView::updateScrollerPosition() {/*
  m_ghostCursor->LocalProperties().AlphaMask() = GHOST_OPACITY;
  
  if ( m_lastSelectedWindow == nullptr )
  {
    m_x.SetInitialValue(m_ghostX.Value());
    m_y.SetInitialValue(m_ghostY.Value());
    m_x.SetGoal(m_ghostX.Value());
    m_y.SetGoal(m_ghostY.Value());
  }
  else {
    m_x.SetGoal(m_ghostX.Value());
    m_y.SetGoal(m_ghostY.Value());
  }*/
}

Vector2 CursorView::getWindowCenter(OSWindow& window) {
  Vector2 position(window.GetPosition().x, window.GetPosition().y);
  Vector2 halfSize(window.GetSize().width / 2.0f, window.GetSize().height / 2.0f);
  return position + (halfSize);
}
