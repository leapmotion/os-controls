#include "stdafx.h"
#include "CursorView.h"
#include "GLShaderLoader.h"
#include "RenderFrame.h"
#include "RenderState.h"

#include <iostream>

CursorView::CursorView(float radius, const Color& color) :
m_state(State::INACTIVE)
{
  Translation() = Vector3(400, 400, -1);
  
  m_disk.Material().SetDiffuseLightColor(color);
  m_disk.Material().SetAmbientLightColor(color);
  m_disk.Material().SetAmbientLightingProportion(0.9f);
  
  m_disk.SetRadius(radius);
}

void CursorView::Move(float x, float y) {
  Translation().x() = x;
  Translation().y() = y;
}

void CursorView::SetSize(float radius) {
  m_disk.SetRadius(radius);
}

void CursorView::InitChildren() {
  //TODO
}

CursorView::~CursorView() {
  RemoveFromParent();
}

void CursorView::AutoInit() {
  auto self = shared_from_this();
  m_rootNode->AddChild(self);
}

void CursorView::AutoFilter(OSCState appState, const HandLocation& handLocation) {
  switch(m_state) {
    case State::INACTIVE:
      if(appState != OSCState::FINAL) {
        m_state = State::ACTIVE;
        m_opacity.Set(1.0f);
      }
      break;
    case State::ACTIVE:
      if(appState == OSCState::FINAL) {
        m_state = State::INACTIVE;
        m_opacity.Set(0.0f);
      }
      break;
  }
  
  switch(m_state) {
    case State::ACTIVE:
      Move(handLocation.x, handLocation.y);
      break;
    case State::INACTIVE:
    default:
      break;
  }
}

void CursorView::AnimationUpdate(const RenderFrame &frame) {
  m_opacity.Update(frame.deltaT.count());
  
  Color color = m_disk.Material().DiffuseLightColor();
  color.A() = m_opacity.Current();
  m_disk.Material().SetDiffuseLightColor(color);
  m_disk.Material().SetAmbientLightColor(color);
}

void CursorView::Render(const RenderFrame& frame) const {
  if (m_disk.Material().DiffuseLightColor().A() == 0.0f)
    return;
  
  // draw primitives
  m_disk.Draw(frame.renderState);
}
