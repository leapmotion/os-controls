#include "stdafx.h"
#include "RenderFrame.h"
#include "volumeKnob.h"

VolumeKnob::VolumeKnob()
  :
  m_childrenAdded(false),
  m_knobBody(std::make_shared<Disk>()),
  m_knobIndicator(std::make_shared<RectanglePrim>())
{
  m_alphaMask.SetInitialValue(0.0f);
  m_alphaMask.SetGoal(0.0f);

  m_knobBody->SetRadius(55.0f);
  m_knobBody->Translation() = Vector3(0, 0, 0);
  m_knobBody->Material().SetAmbientLightingProportion(1.0f);
  
  m_knobIndicator->SetSize(Vector2(2.0f, 110.0));
  m_knobIndicator->Translation() = Vector3(0, -5, 0);
  m_knobIndicator->Material().SetAmbientLightingProportion(1.0f);
}

VolumeKnob::~VolumeKnob() {
  
}

void VolumeKnob::Tick(std::chrono::duration<double> deltaT) {
  AddChildrenIfNecessary();

  m_alphaMask.Update(deltaT.count());
  
  const Color knobColor(0.5f, 0.5f, 0.5f, m_alphaMask.Value());
  const Color indicatorColor(0.505f, 0.831f, 0.114f, m_alphaMask.Value());

  // Because the material ambient lighting proportion is 1,
  // there is no contribution from the diffuse light color.
  m_knobBody->Material().SetAmbientLightColor(knobColor);  
  m_knobIndicator->Material().SetAmbientLightColor(indicatorColor);
}

void VolumeKnob::SetAlphaMaskGoal(float alphaMaskGoal) {
  m_alphaMask.SetGoal(alphaMaskGoal);
}

void VolumeKnob::DrawContents(RenderState &render_state) const {
  // m_knobBody->Draw(render_state);
  // m_knobIndicator->Draw(render_state);
}

void VolumeKnob::AddChildrenIfNecessary() {
  if (!m_childrenAdded) {
    AddChild(m_knobBody);
    AddChild(m_knobIndicator);
    m_childrenAdded = true;
  }
}
