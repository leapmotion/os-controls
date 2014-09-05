#include "stdafx.h"
#include "RenderFrame.h"
#include "volumeKnob.h"

VolumeKnob::VolumeKnob() {
  m_opacity.SetInitialValue(0.0f);
  
  m_knobIndicator.SetSize(Vector2(2.0f, 80.0));
  m_knobIndicator.Translation() = Vector3(0, 30, 0);
  m_knobIndicator.Material().SetAmbientLightColor(Color(255.0f, 255.0f, 255.0f));
  //m_knobIndicator.Material().SetDiffuseLightColor(Color(100.0f, 255.0f, 100.0f, 1.0f));
  
  m_knobBody.SetRadius(40.0f);
  m_knobIndicator.Translation() = Vector3(0, 0, 0);
  m_knobBody.Material().SetAmbientLightColor(Color(128.0f, 128.0f, 128.0f));
  //m_knobBody.Material().SetDiffuseLightColor(Color(128.0f, 128.0f, 128.0f, 0.7f));
}

VolumeKnob::~VolumeKnob() {
  
}

void VolumeKnob::Tick(std::chrono::duration<double> deltaT) {
  m_opacity.Update(deltaT.count());
  
  const Color knobColor(65.0f, 65.0f, 65.0f, m_opacity.Value());
  const Color indicatiorColor(20.0f, 255.0f, 20.0f, m_opacity.Value());
  
  m_knobBody.Material().SetDiffuseLightColor(knobColor);
  m_knobBody.Material().SetAmbientLightColor(knobColor);
  
  m_knobIndicator.Material().SetDiffuseLightColor(indicatiorColor);
  m_knobIndicator.Material().SetAmbientLightColor(indicatiorColor);
}

void VolumeKnob::Draw(RenderState &render_state) const {
  m_knobBody.Draw(render_state);
  m_knobIndicator.Draw(render_state);
}

void VolumeKnob::SetOpacity(float opacity) {
  m_opacity.SetGoal(opacity);
}