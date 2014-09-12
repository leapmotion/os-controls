#pragma once
#include "RenderEngine.h"
#include "GLShader.h"
#include "GLShaderLoader.h"
#include "GLTexture2.h"
#include "TextFile.h"
#include "Resource.h"
#include "Primitives.h"
#include <SVGPrimitive.h>
#include <memory>
#include <Animation.h>

class VolumeSliderView :
  public PrimitiveBase
{
public:

  VolumeSliderView();
  virtual~VolumeSliderView() {};
  
  // Have the menu display its active or inactive state
  void Activate();
  void Deactivate();
  
  // Set the actual system volume via an event call
  void NudgeVolumeLevel(float dLevel);
  
  //Set the visual display volume of the view.
  void SetViewVolume(float volumeLevel);
  
  // Set the width and height of the menu
  void SetWidth(float newWidth);
  void SetHeight(float newHeight);
  
  // Get the width and height of the view
  float Width() const { return m_width; }
  float Height() const { return m_height; }
  float GetNotchOffset() const;
  
  void Update(const RenderFrame& frame);
  
  // Implement PrimitiveBase
  void DrawContents(RenderState &render_state) const override;
  
private:
  const Color INACTIVE_PART_COLOR = Color(0.4f, 0.425f, 0.45f, 0.75f);
  const Color ACTIVE_PART_COLOR = Color(0.505f, 0.831f, 0.114f, 0.95f);
  
  float m_volumeLevel;
  
  float m_width;
  float m_height;
  
  std::shared_ptr<RectanglePrim> m_sliderActivePart;
  std::shared_ptr<RectanglePrim> m_sliderInactivePart;
  std::shared_ptr<SVGPrimitive> m_sliderNotchBodyActive;
  std::shared_ptr<SVGPrimitive> m_sliderNotchBodyInactive;
  
  Vector2 m_sliderNotchOffset;
  
  Smoothed<float> m_activationAmount;
};