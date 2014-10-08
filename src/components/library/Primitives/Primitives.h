#pragma once

#include "PrimitiveBase.h"
#include "PrimitiveGeometry.h"
#include "RenderState.h"

class GLTexture2;

class GenericShape : public PrimitiveBase {
public:

  GenericShape(GLenum drawMode = GL_TRIANGLES) : m_drawMode(drawMode) { }
  virtual ~GenericShape () { }

  // Make sure to call UploadDataToBuffers on the geometry object before drawing.
  PrimitiveGeometry &Geometry () { return m_geometry; }

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  mutable PrimitiveGeometry m_geometry;
  GLenum m_drawMode;
};

class Sphere : public PrimitiveBase {
public:

  Sphere();
  virtual ~Sphere () { }

  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  double m_Radius;
};

class Cylinder : public PrimitiveBase {
public:

  Cylinder();
  virtual ~Cylinder () { }
  
  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  double Height() const { return m_Height; }
  void SetHeight(double height) { m_Height = height; }

  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  double m_Radius;
  double m_Height;
};

class Box : public PrimitiveBase {
public:

  Box();
  virtual ~Box() { }
  
  const EigenTypes::Vector3& Size() const { return m_Size; }
  void SetSize(const EigenTypes::Vector3& size) { m_Size = size; }

  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  EigenTypes::Vector3 m_Size;
};

class Disk : public PrimitiveBase {
public:

  Disk();
  virtual ~Disk () { }
  
  double Radius() const { return m_Radius; }
  void SetRadius(double radius) { m_Radius = radius; }

  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  double m_Radius;
};

class RectanglePrim : public PrimitiveBase {
public:

  RectanglePrim();
  virtual ~RectanglePrim() { }
  
  const EigenTypes::Vector2& Size() const { return m_Size; }
  void SetSize(const EigenTypes::Vector2& size) { m_Size = size; }

  const std::shared_ptr<GLTexture2> &Texture () const { return m_texture; }
  void SetTexture (const std::shared_ptr<GLTexture2> &texture) { m_texture = texture; }

  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  EigenTypes::Vector2 m_Size;
  std::shared_ptr<GLTexture2> m_texture;
};

// This is a textured RectanglePrim which sets its aspect ratio based on the texture.
// It also sets its x/y scale to the image width/height in pixels.
class ImagePrimitive : public RectanglePrim {
public:
  
  ImagePrimitive(void);
  ImagePrimitive(const std::shared_ptr<GLTexture2> &texture);
  virtual ~ImagePrimitive() { }
  
  void SetScaleBasedOnTextureSize ();
};

class PartialDisk : public PrimitiveBase {
public:

  PartialDisk();
  virtual ~PartialDisk() { }

  double InnerRadius() const { return m_InnerRadius; }
  void SetInnerRadius(double innerRad) {
    if (m_InnerRadius != innerRad) {
      m_RecomputeGeometry = true;
    }
    m_InnerRadius = innerRad;
  }

  double OuterRadius() const { return m_OuterRadius; }
  void SetOuterRadius(double outerRad) {
    if (m_OuterRadius != outerRad) {
      m_RecomputeGeometry = true;
    }
    m_OuterRadius = outerRad;
  }

  double StartAngle() const { return m_StartAngle; }
  void SetStartAngle(double startAngleRadians) {
    if (m_StartAngle != startAngleRadians) {
      m_RecomputeGeometry = true;
    }
    m_StartAngle = startAngleRadians;
  }
  
  double EndAngle() const { return m_EndAngle; }
  void SetEndAngle(double endAngleRadians) {
    if (m_EndAngle != endAngleRadians) {
      m_RecomputeGeometry = true;
    }
    m_EndAngle = endAngleRadians;
  }

protected:

  virtual void DrawContents(RenderState& renderState) const override;

protected:

  virtual void RecomputeGeometry() const;

  // cache the previously drawn geometry for speed if the primitive parameters are unchanged
  mutable PrimitiveGeometry m_Geometry;

  mutable bool m_RecomputeGeometry;

  double m_InnerRadius;
  double m_OuterRadius;
  double m_StartAngle;
  double m_EndAngle;
};

class PartialDiskWithTriangle : public PartialDisk {
public:

  PartialDiskWithTriangle();
  
  enum TriangleSide { INSIDE, OUTSIDE };

  void SetTriangleSide(TriangleSide side) {
    if (m_TriangleSide != side) {
      m_RecomputeGeometry = true;
    }
    m_TriangleSide = side;
  }

  void SetTrianglePosition(double pos) {
    if (m_TrianglePosition != pos) {
      m_RecomputeGeometry = true;
    }
    m_TrianglePosition = pos;
  }

  void SetTriangleWidth(double width) {
    if (m_TriangleWidth != width) {
      m_RecomputeGeometry = true;
    }
    m_TriangleWidth = width;
  }

  void SetTriangleOffset(double offset) {
    if (m_TriangleOffset != offset) {
      m_RecomputeGeometry = true;
    }
    m_TriangleOffset = offset;
  }

protected:

  virtual void RecomputeGeometry() const override;

  TriangleSide m_TriangleSide;
  double m_TrianglePosition;
  double m_TriangleWidth;
  double m_TriangleOffset;
};