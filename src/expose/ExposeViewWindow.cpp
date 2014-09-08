#include "stdafx.h"
#include "ExposeViewWindow.h"
#include "osinterface/OSWindow.h"
#include "graphics/RenderFrame.h"

ExposeViewWindow::ExposeViewWindow(OSWindow& osWindow):
  m_osWindow(osWindow.shared_from_this()),
  m_texture(new ImagePrimitive)
{
  m_opacity.SetInitialValue(0.0f);
  m_opacity.SetGoal(0.0f);
  m_opacity.SetSmoothStrength(0.9f);

  m_position.SetInitialValue(Vector3::Zero());
  m_position.SetGoal(Vector3::Zero());
  m_position.SetSmoothStrength(0.85f);

  m_scale.SetInitialValue(0.0f);
  m_scale.SetGoal(0.0f);

  m_activation.SetInitialValue(0.0f);
  m_activation.SetGoal(0.0f);
  m_activation.SetSmoothStrength(0.7f);

  m_hover.SetInitialValue(0.0f);
  m_hover.SetGoal(0.0f);
  m_hover.SetSmoothStrength(0.3f);

  m_grabDelta.SetGoal(Vector3::Zero());
  m_grabDelta.SetInitialValue(Vector3::Zero());
  m_grabDelta.SetSmoothStrength(0.25f);
}

ExposeViewWindow::~ExposeViewWindow(void) {}

void ExposeViewWindow::UpdateTexture(void) {
  m_texture = m_osWindow->GetWindowTexture(m_texture);
}

void ExposeViewWindow::Render(const RenderFrame& frame) const {
  m_texture->DrawSceneGraph(*m_texture, frame.renderState);
}