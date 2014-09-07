#include "stdafx.h"
#include "RenderFrame.h"
#include "volumeKnob.h"

VolumeKnob::VolumeKnob() {
  m_opacity.SetInitialValue(0.0f);
  
  m_knobBody.SetRadius(55.0f);
  m_knobBody.Translation() = Vector3(0, 0, 0);
  
  m_knobIndicator.SetSize(Vector2(2.0f, 110.0));
  m_knobIndicator.Translation() = Vector3(0, -5, 0);
}

VolumeKnob::~VolumeKnob() {
  
}

void VolumeKnob::Tick(std::chrono::duration<double> deltaT) {
  m_opacity.Update(deltaT.count());
  
  const Color knobColor(0.5f, 0.5f, 0.5f, m_opacity.Value());
  const Color indicatiorColor(0.5f, 1.0f, 0.5f, m_opacity.Value());
  
  m_knobBody.Material().SetAmbientLightingProportion(1.0f);
  m_knobBody.Material().SetDiffuseLightColor(knobColor);
  m_knobBody.Material().SetAmbientLightColor(knobColor);
  
  m_knobIndicator.Material().SetAmbientLightingProportion(1.0f);
  m_knobIndicator.Material().SetDiffuseLightColor(indicatiorColor);
  m_knobIndicator.Material().SetAmbientLightColor(indicatiorColor);
}

void VolumeKnob::Draw(RenderState &render_state) const {
  DrawSceneGraph(m_knobBody, render_state);
  DrawSceneGraph(m_knobIndicator, render_state);
}

void VolumeKnob::SetOpacity(float opacity) {
  m_opacity.SetGoal(opacity);
}