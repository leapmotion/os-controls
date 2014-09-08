#include "stdafx.h"
#include "CursorView.h"
#include "GLShaderLoader.h"
#include "RenderEngine.h"
#include "RenderFrame.h"
#include "RenderState.h"
#include "HandCursor.h"

#include <iostream>

CursorView::CursorView() :
  Renderable{OSVector2(400, 400)},
  m_state(State::INACTIVE),
  m_opacity(0.0f, 0.2, EasingFunctions::QuadInOut<float>),
  m_handCursor(new HandCursor())
{

}

CursorView::~CursorView() {
}

void CursorView::SetSize(float radius) {
  //m_disk.SetRadius(radius);
}

void CursorView::AutoInit() {
  m_handCursor->InitChildren();
  m_renderEngine->Add(shared_from_this());
  
}

void CursorView::AutoFilter(const Leap::Hand& hand, OSCState appState, const HandPose& handPose, const HandLocation& handLocation) {
  //State Transitions
  switch(m_state) {
    case State::INACTIVE:
      if(appState != OSCState::FINAL &&
         handPose != HandPose::Clawed) {
        m_state = State::ACTIVE;
        m_opacity.Set(1.0f);
      }
      break;
    case State::ACTIVE:
      if(appState == OSCState::FINAL ||
         handPose == HandPose::Clawed) {
        m_state = State::INACTIVE;
        m_opacity.Set(0.0f);
      }
      break;
  }
  
  //State Loops
  switch(m_state) {
    case State::ACTIVE:
      position = OSVector2{handLocation.x, handLocation.y};
      m_handCursor->Update(hand);
      break;
    case State::INACTIVE:
    default:
      break;
  }
}

void CursorView::AnimationUpdate(const RenderFrame &frame) {
  m_opacity.Update(frame.deltaT.count());
}

void CursorView::Render(const RenderFrame& frame) const {
  switch ( m_state ) {
    case State::ACTIVE:
      PrimitiveBase::DrawSceneGraph(*m_handCursor, frame.renderState);
      break;
    case State::INACTIVE:
    default:
      break;
  }
}
