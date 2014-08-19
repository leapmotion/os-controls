#pragma once

#include "Primitives.h"

struct NSVGimage;

class SVGPrimitive : public PrimitiveBase {
public:

  SVGPrimitive(const std::string& svg = "");
  virtual ~SVGPrimitive();

  void Set(const std::string& svg);

protected:

  virtual void Draw(RenderState& renderState, TransformStack& transform_stack) const override;

private:

  void RecomputeChildren();

  NSVGimage* m_Image;
  bool m_RecomputeGeometry;
};
