#pragma once

#include "Primitives.h"

struct NSVGimage;

class SVGPrimitive : public PrimitiveBase {
public:

  SVGPrimitive(const std::string& svg = "");
  virtual ~SVGPrimitive();

  void Set(const std::string& svg);

  const Vector2& Origin() const { return m_Origin; }
  const Vector2& Size() const { return m_Size; }

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  void RecomputeChildren();

  NSVGimage* m_Image;
  Vector2 m_Origin;
  Vector2 m_Size;
  bool m_RecomputeGeometry;
};
