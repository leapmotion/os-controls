#include "stdafx.h"
#include "ExposeViewWindow.h"
#include "osinterface/OSWindow.h"
#include "osinterface/OSApp.h"
#include "graphics/RenderFrame.h"

ExposeViewWindow::ExposeViewWindow(OSWindow& osWindow):
  m_osWindow(osWindow.shared_from_this()),
  m_texture(new ImagePrimitive),
  m_icon(new ImagePrimitive)
{
  const float randNum = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  const float randomSmoothVariationRadius = 0.05f;
  const float randomVariation = 2*(randNum - 0.5f) * randomSmoothVariationRadius;
  const float baseSmooth = 0.875f;
  const float variedSmooth = baseSmooth + randomVariation;

  m_opacity.SetInitialValue(0.0f);
  m_opacity.SetGoal(0.0f);
  m_opacity.SetSmoothStrength(variedSmooth);

  m_position.SetInitialValue(Vector3::Zero());
  m_position.SetGoal(Vector3::Zero());
  m_position.SetSmoothStrength(variedSmooth);

  m_scale.SetInitialValue(0.0f);
  m_scale.SetGoal(0.0f);
  m_scale.SetSmoothStrength(variedSmooth);

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
  if (m_osWindow->GetOwnerApp()) {
    m_icon = m_osWindow->GetOwnerApp()->GetIconTexture(m_icon);
  }
}

void ExposeViewWindow::Render(const RenderFrame& frame) const {
  m_texture->DrawSceneGraph(*m_texture, frame.renderState);
  m_icon->Translation() = m_texture->Translation();
  m_icon->LinearTransformation() = m_texture->LinearTransformation();
  m_icon->DrawSceneGraph(*m_icon, frame.renderState);
  m_icon->LocalProperties().AlphaMask() = m_texture->LocalProperties().AlphaMask();
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
