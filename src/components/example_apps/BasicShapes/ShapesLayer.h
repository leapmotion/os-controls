#pragma once

#include "DropShadow.h"
#include "Primitives.h"
#include "RenderableEventHandler.h"
#include "RenderState.h"
#include "SVGPrimitive.h"
#include "TexturedFrame.h"

#include <memory>

namespace Leap {
namespace GL {

class Shader;

} // end of namespace GL
} // end of namespace Leap

// This is one "layer" of the application -- layers being rendered back to front.
class ShapesLayer : public RenderableEventHandler {
public:

  ShapesLayer ();
  virtual ~ShapesLayer ();

  virtual void Update (TimeDelta real_time_delta) override;
  virtual void Render (TimeDelta real_time_delta) const override;

  void SetDimensions(int width, int height) { m_Width = width; m_Height = height; }

private:

  int m_Width;
  int m_Height;
  Sphere m_Sphere1;
  Sphere m_Sphere2;
  RectanglePrim m_Rect;
  ImagePrimitive m_Image1;
  ImagePrimitive m_Image2;
  ImagePrimitive m_Image3;
  Disk m_Disk;
  Cylinder m_Cylinder;
  Box m_Box;
  PartialDisk m_PartialDisk;
  SVGPrimitive m_SVG;

  TexturedFrame m_TexturedFrameM; // "middle"
  TexturedFrame m_TexturedFrameE; // "east"
  TexturedFrame m_TexturedFrameN; // "north"
  TexturedFrame m_TexturedFrameW; // "west"
  TexturedFrame m_TexturedFrameS; // "south"
  
  RectanglePrim m_DropShadowContrastBackground;
  DropShadow m_DropShadow;

  mutable RenderState m_Renderer;

  std::shared_ptr<Leap::GL::Shader> m_shader;
  TimePoint m_time;
};
