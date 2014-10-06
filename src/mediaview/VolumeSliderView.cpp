#include "stdafx.h"
#include "VolumeSliderView.h"
#include "graphics/RenderFrame.h"

VolumeSliderView::VolumeSliderView() :
  m_volumeLevel(0.0f),
  m_width(100.0f),
  m_height(20.0f),
  m_sliderActivePart(new RectanglePrim()),
  m_sliderInactivePart(new RectanglePrim()),
  m_sliderNotchBodyActive(new SVGPrimitive()),
  m_sliderNotchBodyInactive(new SVGPrimitive()),
  m_volumeIcon(new SVGPrimitive()),
  m_plusIcon(new SVGPrimitive()),
  m_minusIcon(new SVGPrimitive()),
  m_activationAmount(0.0f,0.75f)
{
  // Setup SVGs
  Resource<TextFile> volumeNotchActiveFile("volume-notch-active.svg");
  Resource<TextFile> volumeNotchInactiveFile("volume-notch-inactive.svg");
  Resource<TextFile> volumeIconFile("volume-icon-01.svg");
  Resource<TextFile> plusIconFile("plus-icon.svg");
  Resource<TextFile> minusIconFile("minus-icon.svg");

  // Assign SVG data to proper primitives.
  m_sliderNotchBodyActive->Set(volumeNotchActiveFile->Contents());
  m_sliderNotchBodyInactive->Set(volumeNotchInactiveFile->Contents());
  m_volumeIcon->Set(volumeIconFile->Contents());
  m_plusIcon->Set(plusIconFile->Contents());
  m_minusIcon->Set(minusIconFile->Contents());

  // Calulate the offsets to the svg primitive centers.
  m_sliderNotchOffset = m_sliderNotchBodyActive->Origin() - (m_sliderNotchBodyActive->Size()/2.0);
  m_volumeIconOffset = m_volumeIcon->Origin() - (m_volumeIcon->Size()/2.0);
  m_plusIconOffset = m_plusIcon->Origin() - (m_plusIcon->Size()/2.0);
  m_minusIconOffset = m_minusIcon->Origin() - (m_minusIcon->Size()/2.0);

  //Setup Rectangle Bars
  m_sliderActivePart->SetSize(EigenTypes::Vector2(0.0f, m_height));
  m_sliderInactivePart->SetSize(EigenTypes::Vector2(0.0f, m_height));

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

void VolumeSliderView::SetOpacity(float opacity) {
  opacity = std::min(1.0f, std::max(0.0f, opacity));
  m_sliderActivePart->LocalProperties().AlphaMask() = opacity;
  m_sliderInactivePart->LocalProperties().AlphaMask() = opacity;
  m_sliderNotchBodyActive->LocalProperties().AlphaMask() = std::min(opacity, static_cast<float>(m_sliderNotchBodyActive->LocalProperties().AlphaMask()));
  m_sliderNotchBodyInactive->LocalProperties().AlphaMask() = opacity;
  m_volumeIcon->LocalProperties().AlphaMask() = opacity;
  m_plusIcon->LocalProperties().AlphaMask() = opacity;
  m_minusIcon->LocalProperties().AlphaMask() = opacity;
}

EigenTypes::Vector2 VolumeSliderView::GetNotchOffset() const {
  EigenTypes::Vector3 notchPosition = m_sliderNotchBodyActive->Translation();
  return EigenTypes::Vector2(notchPosition.x(), notchPosition.y());
}

void VolumeSliderView::Update(const RenderFrame& frame) {
  //Update Smoothed Values
  m_activationAmount.Update(static_cast<float>(frame.deltaT.count()));

  //Calculate bar positions
  float meterLeftEdge = -(m_width / 2.0f);
  float activeWidth = m_width * m_volumeLevel;
  float inactiveWidth = m_width - activeWidth;
  EigenTypes::Vector2 activePosition(meterLeftEdge + (activeWidth/2.0f), 0.0f);
  EigenTypes::Vector2 inactivePosition(meterLeftEdge + activeWidth + (inactiveWidth / 2.0f), 0.0f);

  //Set Bar Positions
  m_sliderActivePart->SetSize(EigenTypes::Vector2(activeWidth, m_height));
  m_sliderInactivePart->SetSize(EigenTypes::Vector2(inactiveWidth, m_height));
  m_sliderActivePart->Translation() = EigenTypes::Vector3(activePosition.x(), activePosition.y(), 0.0f);
  m_sliderInactivePart->Translation() = EigenTypes::Vector3(inactivePosition.x(), inactivePosition.y(), 0.0f);

  //Calculate notch position
  EigenTypes::Vector2 sliderNotchBodyPosition(meterLeftEdge + m_sliderNotchOffset.x() + (m_width * m_volumeLevel), m_sliderNotchOffset.y());

  //Set Active Notch Opacity
  m_sliderNotchBodyActive->LocalProperties().AlphaMask() = m_activationAmount;

  //Set notchPosition
  m_sliderNotchBodyInactive->Translation() = EigenTypes::Vector3(sliderNotchBodyPosition.x(), sliderNotchBodyPosition.y(),0.0f);
  m_sliderNotchBodyActive->Translation() = EigenTypes::Vector3(sliderNotchBodyPosition.x(), sliderNotchBodyPosition.y(),0.0f);

  //Set icon positons
  m_minusIcon->Translation() = EigenTypes::Vector3(meterLeftEdge + m_minusIconOffset.x(), ICON_Y_OFFSET + m_minusIconOffset.y(), 0.0f);
  m_plusIcon->Translation() = EigenTypes::Vector3(meterLeftEdge + m_width + m_plusIconOffset.x(), ICON_Y_OFFSET + m_plusIconOffset.x(), 0.0f);
  m_volumeIcon->Translation() = EigenTypes::Vector3(meterLeftEdge + (m_width/2.0) + m_volumeIconOffset.x(), ICON_Y_OFFSET + m_volumeIconOffset.y(), 0.0f);
}

void VolumeSliderView::DrawContents(RenderState &render_state) const {
  // Draw bar
  PrimitiveBase::DrawSceneGraph(*m_sliderActivePart, render_state);
  PrimitiveBase::DrawSceneGraph(*m_sliderInactivePart, render_state);

  //Draw notch body.
  PrimitiveBase::DrawSceneGraph(*m_sliderNotchBodyInactive, render_state);
  PrimitiveBase::DrawSceneGraph(*m_sliderNotchBodyActive, render_state);

  //Draw icons.
  PrimitiveBase::DrawSceneGraph(*m_volumeIcon, render_state);
  PrimitiveBase::DrawSceneGraph(*m_plusIcon, render_state);
  PrimitiveBase::DrawSceneGraph(*m_minusIcon, render_state);
}
