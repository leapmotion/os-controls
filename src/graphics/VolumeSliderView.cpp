#include "stdafx.h"
#include "VolumeSliderView.h"
#include "RenderFrame.h"

VolumeSliderView::VolumeSliderView() :
  m_volumeLevel(0.0f),
  m_width(100.0f),
  m_height(20.0f),
  m_sliderActivePart(new RectanglePrim()),
  m_sliderInactivePart(new RectanglePrim()),
  m_sliderNotchBodyActive(new SVGPrimitive()),
  m_sliderNotchBodyInactive(new SVGPrimitive())
{
  m_activationAmount.SetInitialValue(0.0f);
  m_activationAmount.SetSmoothStrength(0.8f);
  
  // Setup SVGs
  Resource<TextFile> volumeNotchActiveFile("volume-notch-active.svg");
  Resource<TextFile> volumeNotchInactiveFile("volume-notch-inactive.svg");
  
  // Assign SVG data to proper primitives.
  m_sliderNotchBodyActive->Set(volumeNotchActiveFile->Contents());
  m_sliderNotchBodyInactive->Set(volumeNotchInactiveFile->Contents());
  
  // Calulate the offsets to the svg primitive centers.
  m_sliderNotchOffset = m_sliderNotchBodyActive->Origin() - (m_sliderNotchBodyActive->Size()/2.0);
  
  //Setup Rectangle Bars
  m_sliderActivePart->SetSize(Vector2(0.0f, m_height));
  m_sliderInactivePart->SetSize(Vector2(0.0f, m_height));
  
  //Setup bar materials
  m_sliderActivePart->Material().SetDiffuseLightColor(ACTIVE_PART_COLOR);
  m_sliderActivePart->Material().SetAmbientLightColor(ACTIVE_PART_COLOR);
  m_sliderActivePart->Material().SetAmbientLightingProportion(1.0f);
  
  m_sliderInactivePart->Material().SetDiffuseLightColor(INACTIVE_PART_COLOR);
  m_sliderInactivePart->Material().SetAmbientLightColor(INACTIVE_PART_COLOR);
  m_sliderInactivePart->Material().SetAmbientLightingProportion(1.0f);
}

void VolumeSliderView::Activate() {
  m_activationAmount.SetGoal(1.0f);
}

void VolumeSliderView::Deactivate() {
  m_activationAmount.SetGoal(0.0f);
}

void VolumeSliderView::NudgeVolumeLevel(float dLevel) {
  m_mediaEvents(&MediaViewEventListener::OnUserChangedVolume)(dLevel);
}

void VolumeSliderView::SetViewVolume(float volumeLevel) {
  m_volumeLevel = std::min(1.0f, std::max(0.0f, volumeLevel)); // Clamp incoming volume level
}

void VolumeSliderView::SetWidth(float newWidth) {
  newWidth = std::max(0.0f, newWidth);
  m_width = newWidth;
}

void VolumeSliderView::SetHeight(float newHeight) {
  newHeight = std::max(0.0f, newHeight);
  m_height = newHeight;
}

Vector2 VolumeSliderView::GetNotchOffset() const {
  Vector3 notchPosition = m_sliderNotchBodyActive->Translation();
  return Vector2(notchPosition.x(), notchPosition.y());
}

void VolumeSliderView::Update(const RenderFrame& frame) {
  //Update Smoothed Values
  m_activationAmount.Update(frame.deltaT.count());
  
  //Calculate bar positions
  float meterLeftEdge = m_width / 2.0f;
  float activeWidth = m_width * m_volumeLevel;
  float inactiveWidth = m_width * (1 - m_volumeLevel);
  Vector2 activePosition(meterLeftEdge + activeWidth/2.0f, 0.0f);
  Vector2 inactivePosition(meterLeftEdge + activeWidth + inactiveWidth / 2.0f, 0.0f);
  
  //Set Bar Positions
  m_sliderActivePart->SetSize(Vector2(activeWidth, m_height));
  m_sliderInactivePart->SetSize(Vector2(inactiveWidth, m_height));
  m_sliderActivePart->Translation() = Vector3(activePosition.x(), activePosition.y(), 0.0f);
  m_sliderActivePart->Translation() = Vector3(activePosition.x(), activePosition.y(), 0.0f);
  
  //Calculate notch position
  Vector2 sliderNotchBodyPosition(meterLeftEdge + m_sliderNotchOffset.x() + m_width * m_volumeLevel, m_sliderNotchOffset.y());
  
  //Set Active Notch Opacity
  m_sliderNotchBodyActive->LocalProperties().AlphaMask() = m_activationAmount;
  
}

void VolumeSliderView::DrawContents(RenderState &render_state) const {
  // Draw bar
  PrimitiveBase::DrawSceneGraph(*m_sliderActivePart, render_state);
  PrimitiveBase::DrawSceneGraph(*m_sliderInactivePart, render_state);
  
  //Draw notch body.
  PrimitiveBase::DrawSceneGraph(*m_sliderNotchBodyInactive, render_state);
  PrimitiveBase::DrawSceneGraph(*m_sliderNotchBodyActive, render_state);
}