#include "stdafx.h"
#include "Cursor.h"
#include "GLShaderLoader.h"
#include "RenderState.h"

#include <iostream>

Cursor::Cursor(float radius, const Color& color) {
  Translation() = Vector3(400, 400, 0);
  
  m_disk.SetDiffuseColor(color);
  m_disk.SetAmbientFactor(0.9f);
  
  m_disk.SetRadius(radius);
}

void Cursor::Move(float x, float y) {
  Translation().x() = x;
  Translation().y() = y;
}

void Cursor::SetSize(float radius) {
  m_disk.SetRadius(radius);
}

void Cursor::InitChildren() {
  //TODO
}

void Cursor::AnimationUpdate(const RenderFrame &frame) {
  switch (m_fadeState) {
    case FADE_IN:
      setOpacity(1.0f);
      break;
    case FADE_OUT:
      setOpacity(0.0f);
      break;
  }
}

void Cursor::Render(const RenderFrame& frame) const {
  if (m_disk.DiffuseColor().A() == 0.0f)
    return;
  
  // draw primitives
  m_disk.Draw(frame.renderState);
}

void Cursor::SetFadeState(FadeState newState){
  m_fadeState = newState;
}

void Cursor::setOpacity(float opacity) {
  Color color = m_disk.DiffuseColor();
  color.A() = opacity;
  m_disk.SetDiffuseColor(color);
}
