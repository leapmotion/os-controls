#pragma once

#include "LeapListener.h"
#include "Primitives.h"
#include "DropShadow.h"
#include "Leap/GL/Rgba.h"

using namespace Leap::GL;

class HandCursor : public PrimitiveBase {
public:
  enum DrawStyle { RINGED_PAW, BLOB };

  HandCursor();

  void Update(const Leap::Hand& hand);
  void InitChildren();

  void SetDrawStyle(DrawStyle style) { m_DrawStyle = style; }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:
  static const int NUM_FINGERS = 5;
  
  // Computes a "bend amount" for this finger, between 0 (not bent) and 1 (very bent)
  // When pinching, the thumb is forced to be bent
  float fingerBend(const Leap::Finger& finger) const;
  
  void formatFinger(const Leap::Finger& finger, float distance, bool isLeft = false);

  Rgba<float> m_OutlineColor;
  Rgba<float> m_FillColor;

  float m_PalmOutlineRadius;
  float m_PalmOutlineThickness;

  DrawStyle m_DrawStyle;

  std::shared_ptr<PartialDisk> m_PalmOutline;
  std::shared_ptr<Disk> m_PalmCenter;
  std::shared_ptr<Disk> m_Fingers[5];

  std::shared_ptr<DropShadow> m_FingerDropShadows[5];
  std::shared_ptr<DropShadow> m_PalmDropShadow;

};
