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
  m_activation.SetSmoothStrength(0.3f);

  m_hover.SetInitialValue(0.0f);
  m_hover.SetGoal(0.0f);
  m_hover.SetSmoothStrength(0.3f);

  m_selection.SetInitialValue(0.0f);
  m_selection.SetGoal(0.0f);
  m_selection.SetSmoothStrength(0.5f);

  m_grabDelta.SetGoal(Vector3::Zero());
  m_grabDelta.SetInitialValue(Vector3::Zero());
  m_grabDelta.SetSmoothStrength(0.25f);

  m_cooldown = false;
}

ExposeViewWindow::~ExposeViewWindow(void) {}

void ExposeViewWindow::UpdateTexture(void) {
  m_texture = m_osWindow->GetWindowTexture(m_texture);
}

void ExposeViewWindow::Render(const RenderFrame& frame) const {
  m_texture->DrawSceneGraph(*m_texture, frame.renderState);
}

void ExposeViewWindow::SetOpeningPosition() {
  m_opacity.SetInitialValue(1.0f);
  m_opacity.SetGoal(1.0f);

  m_opacity.Update(0.0f);

  m_scale.SetGoal(1.0f);
  m_scale.SetInitialValue(1.0f);
  m_scale.Update(0.0f);

  m_activation.SetGoal(0.0f);
  m_activation.Update(0.0f);

  m_hover.SetGoal(0.0f);
  m_hover.Update(0.0f);

  m_selection.SetGoal(0.0f);
  m_selection.Update(0.0f);

  m_grabDelta.SetGoal(Vector3::Zero());
  m_grabDelta.Update(0.0f);
 
  const Vector2 osPosition = GetOSPosition();
  const Vector3 center(osPosition.x(), osPosition.y(), 0.0);

  m_position.SetInitialValue(center);
  m_position.SetGoal(center);
  m_position.Update(0.0f);
}

void ExposeViewWindow::SetClosingPosition() {
  m_scale.SetGoal(1.0f);

  m_activation.SetGoal(0.0f);

  m_hover.SetGoal(0.0f);

  m_selection.SetGoal(0.0f);

  m_grabDelta.SetGoal(Vector3::Zero());

  const Vector2 osPosition = GetOSPosition();
  const Vector3 center(osPosition.x(), osPosition.y(), 0.0);

  m_position.SetGoal(center);
}

Vector2 ExposeViewWindow::GetOSPosition() const {
  const OSPoint pos = m_osWindow->GetPosition();
  const OSSize size = m_osWindow->GetSize();
  return Vector2(pos.x + 0.5*size.width, pos.y + 0.5*size.height);
}
