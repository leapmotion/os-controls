#pragma once

#include "Primitives.h"

struct NSVGimage;

class SVGPrimitive : public PrimitiveBase {
public:

  SVGPrimitive(const std::string& svg = "");
  virtual ~SVGPrimitive();

  void Set(const std::string& svg);

  virtual void Draw(RenderState& renderState) const override;

private:

  void RecomputeChildren();

  NSVGimage* m_Image;
  bool m_RecomputeGeometry;
};
