#include "stdafx.h"
#include "ExposeViewWindow.h"
#include "osinterface/OSWindow.h"
#include "osinterface/OSApp.h"
#include "graphics/RenderFrame.h"

const double ExposeViewWindow::VIEW_ANIMATION_TIME = 1.0;

static float getRandomVariation(float radius) {
  const float randNum = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
  return 2.0f*(randNum - 0.5f) * radius;
}

ExposeViewWindow::ExposeViewWindow(OSWindow& osWindow):
  m_osWindow(osWindow.shared_from_this()),
  m_texture(new ImagePrimitive),
  m_dropShadow(new DropShadow),
  m_highlight(new RectanglePrim),
  m_position(Vector3::Zero(), VIEW_ANIMATION_TIME, EasingFunctions::QuadInOut<Vector3>),
  m_prevPosition(Vector3::Zero())
{
  m_opacity.SetInitialValue(0.0f);
  m_opacity.SetGoal(0.0f);
  m_opacity.SetSmoothStrength(0.825f);
  m_opacity.Update(0.0f);

  m_position.SetImmediate(Vector3::Zero());
  m_position.Set(Vector3::Zero());

  m_scale.SetInitialValue(0.0f);
  m_scale.SetGoal(0.0f);
  m_scale.SetSmoothStrength(0.825f);
  m_scale.Update(0.0f);

  m_activation.SetInitialValue(0.0f);
  m_activation.SetGoal(0.0f);
  m_activation.SetSmoothStrength(0.3f);
  m_activation.Update(0.0f);

  m_hover.SetInitialValue(0.0f);
  m_hover.SetGoal(0.0f);
  m_hover.SetSmoothStrength(0.5f);
  m_hover.Update(0.0f);

  m_selection.SetInitialValue(0.0f);
  m_selection.SetGoal(0.0f);
  m_selection.SetSmoothStrength(0.5f);
  m_selection.Update(0.0f);

  m_grabDelta.SetGoal(Vector3::Zero());
  m_grabDelta.SetInitialValue(Vector3::Zero());
  m_grabDelta.SetSmoothStrength(0.25f);
  m_grabDelta.Update(0.0f);

  m_forceDelta.SetGoal(Vector3::Zero());
  m_forceDelta.SetInitialValue(Vector3::Zero());
  m_forceDelta.SetSmoothStrength(0.75f);
  m_forceDelta.Update(0.0f);

  m_velocity.SetGoal(Vector3::Zero());
  m_velocity.SetInitialValue(Vector3::Zero());
  m_velocity.SetSmoothStrength(0.65f);
  m_velocity.Update(0.0f);
}

ExposeViewWindow::~ExposeViewWindow(void) {}

void ExposeViewWindow::UpdateTexture(void) {
  m_texture = m_osWindow->GetWindowTexture(m_texture);
}

void ExposeViewWindow::Render(const RenderFrame& frame) const {
  static const Vector3 DROP_SHADOW_OFFSET(3, 5, 0);
  static const double DROP_SHADOW_RADIUS = 50.0;
  static const float DROP_SHADOW_OPACITY = 0.4f;
#if __APPLE__
  const float opacity = DROP_SHADOW_OPACITY;
#else
  const float transition = static_cast<float>(m_position.Completion());
  const float opacity = DROP_SHADOW_OPACITY * (m_closing ? 1.0f - transition : transition);
#endif
  m_dropShadow->Translation() = m_texture->Translation() + DROP_SHADOW_OFFSET;
  m_dropShadow->SetBasisRectangleSize(m_texture->Size());
  m_dropShadow->LinearTransformation() = m_texture->LinearTransformation();
  m_dropShadow->SetShadowRadius(DROP_SHADOW_RADIUS);
  m_dropShadow->LocalProperties().AlphaMask() = opacity * m_texture->LocalProperties().AlphaMask();
  PrimitiveBase::DrawSceneGraph(*m_dropShadow, frame.renderState);

  static const double HIGHLIGHT_WIDTH = 50.0;
  m_highlight->LocalProperties().AlphaMask() = m_activation.Value();
  const Vector3f highlightRGB(0.505f, 0.831f, 0.114f);
  Color highlightColor(highlightRGB);
  m_highlight->Material().SetDiffuseLightColor(highlightColor);
  m_highlight->Material().SetAmbientLightColor(highlightColor);
  m_highlight->Material().SetAmbientLightingProportion(1.0f);
  Vector2 size = m_texture->Size();
  size += Vector2::Constant(HIGHLIGHT_WIDTH);
  m_highlight->SetSize(size);
  m_highlight->Translation() = m_texture->Translation();
  m_highlight->LinearTransformation() = m_texture->LinearTransformation();

  PrimitiveBase::DrawSceneGraph(*m_highlight, frame.renderState);

  m_texture->DrawSceneGraph(*m_texture, frame.renderState);
}

void ExposeViewWindow::SetOpeningPosition() {
  m_closing = false;

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
 
  m_forceDelta.SetGoal(Vector3::Zero());
  m_forceDelta.Update(0.0f);

  const Vector2 osPosition = GetOSPosition();
  const Vector3 center(osPosition.x(), osPosition.y(), 0.0);

#if 0
  m_position.SetInitialValue(center);
  m_position.SetGoal(center);
#else
  const float randomTimeVariation = 0.15f;

  m_position.SetImmediate(center);
  m_position.Set(center, 0.95*VIEW_ANIMATION_TIME - randomTimeVariation + getRandomVariation(randomTimeVariation));
  m_prevPosition = center;
#endif
}

void ExposeViewWindow::SetClosingPosition() {
  m_closing = true;

  m_scale.SetGoal(1.0f);

  m_activation.SetGoal(0.0f);

  m_hover.SetGoal(0.0f);

  m_selection.SetGoal(0.0f);

  m_grabDelta.SetGoal(Vector3::Zero());

  m_forceDelta.SetGoal(Vector3::Zero());

  const Vector2 osPosition = GetOSPosition();
  const Vector3 center(osPosition.x(), osPosition.y(), 0.0);

#if 0
  m_position.SetGoal(center);
#else
  const float randomTimeVariation = 0.15f;
  m_position.Set(center, 0.95*VIEW_ANIMATION_TIME - randomTimeVariation + getRandomVariation(randomTimeVariation));
#endif
}

Vector2 ExposeViewWindow::GetOSPosition() const {
  const OSPoint pos = m_osWindow->GetPosition();
  const OSSize size = m_osWindow->GetSize();
  return Vector2(pos.x + 0.5*size.width, pos.y + 0.5*size.height);
}

Vector2 ExposeViewWindow::GetOSSize() const {
  const OSSize size = m_osWindow->GetSize();
  return Vector2(size.width, size.height);
}
