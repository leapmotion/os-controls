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
}

CursorView::~CursorView() {
}

void CursorView::SetSize(float radius) {
}

void CursorView::AutoInit() {
  m_renderEngine->Add(shared_from_this());
}

void CursorView::AutoFilter(const Leap::Hand& hand, OSCState appState, const HandPose& handPose, const HandLocation& handLocation) {
  m_renderEngine->BringToFront(this);

  //State Transitions
  switch(m_state) {
    case State::INACTIVE:
      if(appState != OSCState::FINAL &&
         handPose != HandPose::Clawed) {
        m_state = State::ACTIVE;
        m_alphaMask.Set(1.0f);
      }
      break;
    case State::ACTIVE:
      if(appState == OSCState::FINAL ||
         handPose == HandPose::Clawed) {
        m_state = State::INACTIVE;
        m_alphaMask.Set(0.0f);
      }
      break;
  }
  
  //State Loops
  switch(m_state) {
    case State::ACTIVE:
      position = OSVector2{handLocation.x, handLocation.y};
      break;
    case State::INACTIVE:
    default:
      break;
  }
}

void CursorView::AnimationUpdate(const RenderFrame &frame) {
  m_alphaMask.Update(frame.deltaT.count());
}

void CursorView::Render(const RenderFrame& frame) const {
  switch ( m_state ) {
    case State::ACTIVE:
      break;
    case State::INACTIVE:
    default:
      break;
  }
}
