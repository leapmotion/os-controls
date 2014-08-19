#pragma once

#include "PrimitiveBase.h"
#include "PrimitiveGeometry.h"
#include "RenderState.h"

class SVGPrimitive : public PrimitiveBase {
public:

  SVGPrimitive(const std::string& svg);

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
protected:

  virtual void Draw(RenderState& renderState, TransformStack& transform_stack) const override;

private:

  // cache the previously drawn geometry for speed if the primitive parameters are unchanged
  mutable PrimitiveGeometry m_Geometry;

};
