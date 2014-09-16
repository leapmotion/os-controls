#include "stdafx.h"
#include "CursorView.h"
#include "graphics/RenderEngine.h"
#include "graphics/RenderFrame.h"
#include "utility/Config.h"

#include "GLShaderLoader.h"
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
  m_scrollBodyOffset(0,0),
  m_scrollLineOffset(0,0),
  m_scrollFingerLeftOffset(0,0),
  m_scrollFingerRightOffset(0,0),
  m_disabledCursorOffset(0,0),
  m_scrollBody(new SVGPrimitive()),
  m_scrollLine(new SVGPrimitive()),
  m_scrollFingerLeft(new SVGPrimitive()),
  m_scrollFingerRight(new SVGPrimitive()),
  m_disabledCursor(new SVGPrimitive()),
  m_disk(new Disk()),
  m_fingerSpread(0.0f),
  m_pinchStrength(0.0f),
  m_lastHandDeltas(0,0),
  m_lastHandPosition(0,0),
  m_overrideX(0.0f),
  m_overrideY(0.0f),
  m_overrideInfluence(0.0f)
{
  const Color CURSOR_COLOR(0.505f, 0.831f, 0.114f, 0.95f);
  
  // Initialize Smoothed member variables.
  m_bodyOffset.SetInitialValue(0.0f);
  m_bodyOffset.SetSmoothStrength(0.8f);
  
  m_x.SetSmoothStrength(0.0f);
  m_y.SetSmoothStrength(0.0f);
  
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
  Resource<TextFile> disabledCursorFile("disabled-cursor.svg");
  
  // Assign SVG data to proper primitives.
  m_scrollBody->Set(scrollBodyFile->Contents());
  m_scrollLine->Set(scrollLineFile->Contents());
  m_scrollFingerLeft->Set(scrollFingerRightFile->Contents());
  m_scrollFingerRight->Set(scrollFingerLeftFile->Contents());
  m_disabledCursor->Set(disabledCursorFile->Contents());
  
  // Calulate the offsets to the svg primitive centers.
  m_scrollBodyOffset = m_scrollBody->Origin() - (m_scrollBody->Size()/2.0);
  m_scrollLineOffset = m_scrollLine->Origin() - (m_scrollLine->Size()/2.0);
  m_scrollFingerLeftOffset = m_scrollFingerLeft->Origin() - (m_scrollFingerLeft->Size()/2.0);
  m_scrollFingerRightOffset = m_scrollFingerRight->Origin() - (m_scrollFingerRight->Size()/2.0);
  m_disabledCursorOffset = m_disabledCursor->Origin() - (m_disabledCursor->Size()/2.0);

  AutoCurrentContext()->AddTeardownListener([this](){
    RemoveFromParent();
  });
}

CursorView::~CursorView() {
}

void CursorView::SetSize(float radius) {
}

void CursorView::AutoInit() {
  m_renderEngine->Add(shared_from_this());
}

void CursorView::AutoFilter(const Leap::Hand& hand, OSCState appState, const HandData& handData, const FrameTime& frameTime) {
  static const float FINGER_SPREAD_MIN = 23.0f; // The min spread that the "finger arrow" visuals should have
  static const float FINGER_SPREAD_MAX = 100.0f;// The max spread that the "finger arrow" visuals should have
  static const float SCROLL_VELOCITY_MIN = 0.0f; // The low bound of scroll velocity for our visual feedback ( the handle moving up and down )
  static const float SCROLL_VELOCITY_MAX = 20.0f; // The high bound of scroll velcity for our visual feedback ( the handle moving up and down )
  static const float BODY_OFFSET_MAX = 100.0f; // The maximum distance the handle will move up or down along the cursor
  
  int velocitySign = 1;
  float velocityNorm = 0.0f;
  float goalBodyOffset = 0.0f;
  
  // Make sure the cursor is frontmost in its rendering context
  if (appState != OSCState::FINAL) {
    m_renderEngine->BringToFront(this);
  } else {
    // NOTE: This is probably not called.
    // Instead, the callback set in AddTearDownListener will be called
    RemoveFromParent();
  }
  
  m_lastAppState = appState;

  //State Transitions
  switch(m_state) {
    case State::INACTIVE:
      if(appState != OSCState::FINAL ) {
        m_state = State::ACTIVE;
      }
      break;
    case State::ACTIVE:
      if(appState == OSCState::FINAL) {
        m_state = State::INACTIVE;
      }
    case State::DISABLED:
      if(appState == OSCState::FINAL) {
        m_state = State::INACTIVE;
      }
      break;
  }
  
  //State Loops
  switch(m_state) {
    case State::ACTIVE:
    case State::DISABLED:
    {
      m_pinchStrength = handData.pinchData.pinchStrength;
      float spreadNorm = m_pinchStrength / activationConfigs::MIN_PINCH_START;
      spreadNorm = std::max(0.0f, std::min(1.0f, spreadNorm));
      m_fingerSpread = FINGER_SPREAD_MIN + ((1-spreadNorm) * (FINGER_SPREAD_MAX - FINGER_SPREAD_MIN));
    
      if ( appState == OSCState::SCROLLING ) {
        // Calculate the offset of the cursor handle based on the current hand deltas
        velocitySign = handData.locationData.dY < 0 ? -1 : 1;
        velocityNorm = (fabs(handData.locationData.dY) - SCROLL_VELOCITY_MIN) / (SCROLL_VELOCITY_MAX - SCROLL_VELOCITY_MIN);
        velocityNorm = std::min(1.0f, std::max(0.0f, velocityNorm));
        velocityNorm *= velocitySign;
        goalBodyOffset = velocityNorm * BODY_OFFSET_MAX;
        m_bodyOffset.SetGoal(goalBodyOffset);
      }
      else {
        // we only want to move the cursor if we're not scrolling so move it in this else
        m_x.SetGoal(handData.locationData.x);
        m_y.SetGoal(handData.locationData.y);
        if ( m_bodyOffset.Goal() != 0.0f ) { m_bodyOffset.SetGoal(0.0f); } //Make sure to bring the handle back to center
      }
    
      m_lastHandDeltas = Vector2(handData.locationData.dX, handData.locationData.dY);

      m_lastHandPosition = handData.locationData.screenPosition();
      break;
    }
    case State::INACTIVE:
      m_bodyOffset.SetInitialValue(0.0f);
    default:
      break;
  }
}

Vector2 CursorView::GetCalculatedLocation() const {
  return Vector2(m_x,m_y);
}

void CursorView::SetOverideLocation(const Vector2& offsetLocation) {
  // Set the offset location
  m_overrideX = offsetLocation.x();
  m_overrideY = offsetLocation.y();
}

void CursorView::AnimationUpdate(const RenderFrame &frame) {
  if ( m_lastAppState == OSCState::MEDIA_MENU_FOCUSED ||
       m_lastAppState == OSCState::EXPOSE_FOCUSED ||
       m_lastAppState == OSCState::EXPOSE_ACTIVATOR_FOCUSED ||
       !m_config->Get<bool>("enableScroll")) {
    // Don't show the scroll cursor if we're in a state where we can't be scrolling
    // The scroll cursor shows up before we go into the scroll state (as a hint for the user)
    // So we can't just check if we're in the scrolling state
    m_bodyAlpha.SetGoal(0.0f);
  }
  else {
    m_bodyAlpha.SetGoal(1.0f);
  }
  
  // Update smoothed value
  m_bodyAlpha.Update(static_cast<float>(frame.deltaT.count()));
  
  //Set the alpha of the body and the line behind it.
  m_scrollBody->LocalProperties().AlphaMask() = m_bodyAlpha.Value();
  m_scrollLine->LocalProperties().AlphaMask() = m_bodyAlpha.Value();
  
  // Calculate and set the opacity of the fingers
  float fingerOpacity = std::min(1.0f, m_bodyAlpha.Value());
  m_scrollFingerLeft->LocalProperties().AlphaMask() = fingerOpacity;
  m_scrollFingerRight->LocalProperties().AlphaMask() = fingerOpacity;
  
  // Update the smooth strength on the cursor position
  m_x.SetSmoothStrength(calcPositionSmoothStrength(m_lastHandDeltas.norm()));
  m_y.SetSmoothStrength(calcPositionSmoothStrength(m_lastHandDeltas.norm()));
  // Update the smoohted position variables and offset
  m_x.Update(static_cast<float>(frame.deltaT.count()));
  m_y.Update(static_cast<float>(frame.deltaT.count()));
  m_bodyOffset.Update(static_cast<float>(frame.deltaT.count()));
  
  // If another object is overriding our value, we don't want to fight it by setting the location ourself.
  Vector2 baseLocation = Vector2(m_x.Value(), m_y.Value());
  Vector2 cursorLocation = baseLocation + (m_overrideInfluence * (Vector2(m_overrideX, m_overrideY) - baseLocation));
  position = OSVector2{ static_cast<float>(cursorLocation.x()), static_cast<float>(cursorLocation.y()) };

  // If the scroll cursor is fading in/out, fade out/in the disk cursor
  if ( m_bodyAlpha.Value() < 0.2f ) {
    m_diskAlpha.SetGoal(1.0f);
  }
  else {
    m_diskAlpha.SetGoal(0.0f);
  }
  
  // Update and set the alpha value of the disk cursor
  m_diskAlpha.Update(static_cast<float>(frame.deltaT.count()));
  m_disk->LocalProperties().AlphaMask() = m_diskAlpha;
  
  
  // Snapped Scrolling Cursor Positioning
  m_scrollBody->Translation() = Vector3(m_scrollBodyOffset.x(), m_scrollBodyOffset.y()+ m_bodyOffset, 0.0f);
  m_scrollLine->Translation() = Vector3(m_scrollLineOffset.x(), m_scrollLineOffset.y(), 0.0f);
  m_scrollFingerLeft->Translation() = Vector3(m_scrollFingerLeftOffset.x() + m_fingerSpread, m_scrollFingerLeftOffset.y() + m_bodyOffset, 0.0f);
  m_scrollFingerRight->Translation() = Vector3(m_scrollFingerRightOffset.x() - m_fingerSpread, m_scrollFingerRightOffset.y() + m_bodyOffset, 0.0f);
  m_disabledCursor->Translation() = Vector3(m_disabledCursorOffset.x(), m_disabledCursorOffset.y(), 0.0f);
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
    case State::DISABLED:
      PrimitiveBase::DrawSceneGraph(*m_disabledCursor, frame.renderState);
    case State::INACTIVE:
    default:
      break;
  }
}

float CursorView::calcPositionSmoothStrength(float handDeltaDistance) const {
  float retVal = 0.0f;
  
  // hand delta based smoothing
  float normalizedDelta = (handDeltaDistance - DELTA_FOR_MAX_SMOOTHING) / (DELTA_FOR_MIN_SMOOTHING - DELTA_FOR_MAX_SMOOTHING);
  normalizedDelta = 1.0f - std::min(1.0f, std::max(0.0f, normalizedDelta));
  float deltaSmoothing  = MAX_CURSOR_SMOOTHING * normalizedDelta;
  deltaSmoothing = std::min(MAX_CURSOR_SMOOTHING, std::max(0.0f, deltaSmoothing));
  retVal = deltaSmoothing;
  
  return retVal;
}

Vector2 CursorView::getWindowCenter(OSWindow& window) {
  Vector2 position(window.GetPosition().x, window.GetPosition().y);
  Vector2 halfSize(window.GetSize().width / 2.0f, window.GetSize().height / 2.0f);
  return position + (halfSize);
}
