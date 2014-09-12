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
  
  const Vector3& Size() const { return m_Size; }
  void SetSize(const Vector3& size) { m_Size = size; }

  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  Vector3 m_Size;
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
  
  const Vector2& Size() const { return m_Size; }
  void SetSize(const Vector2& size) { m_Size = size; }

  const std::shared_ptr<GLTexture2> &Texture () const { return m_texture; }
  void SetTexture (const std::shared_ptr<GLTexture2> &texture) { m_texture = texture; }

  virtual void MakeAdditionalModelViewTransformations (ModelView &model_view) const override;

protected:

  virtual void DrawContents(RenderState& renderState) const override;

private:

  Vector2 m_Size;
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

// Design notes for TexturedFrame
// ------------------------------
// This primitive will provide a configurable, measured, 9-cell frame of the following form:
//
//   +---+-------------------+---+
//   | 0 |         1         | 2 |
//   +---+-------------------+---+
//   |   |                   |   |
//   | 3 |         4         | 5 |
//   |   |                   |   |
//   +---+-------------------+---+
//   | 6 |         7         | 8 |
//   +---+-------------------+---+
//
// The outer cells constitute the "border" of the frame.  The "outer rectangle" of the frame
// is made up by the outer edges of the border:
//
//   +---+-------------------+---+
//   | 0           1           2 |
//   +                           +
//   |                           |
//   | 3           4           5 |
//   |                           |
//   +                           +
//   | 6           7           8 |
//   +---+-------------------+---+
//
// The "inner rectangle" of the frame is made up by the inner edges of the border:
//
//                                
//     0           1           2  
//       +-------------------+    
//       |                   |    
//     3 |         4         | 5  
//       |                   |    
//       +-------------------+    
//     6           7           8  
//                                
//
// The inner rectangle must not exceed the outer rectangle (though they are allowed to be the same
// size).  The inner and outer rectangles' edges are configurable based on a "basis rectangle", which
// must be specified by the user of this primitive.  The default values are:
// - The inner rectangle will be set to the basis rectangle.
// - The outer rectangle will extend half a unit out from the basis rectangle in each direction.
//
// Each rectangle edge is independent, and so non-symmetric borders can be defined.  The inner and
// outer rectangles are controlled by an offset value for each edge based on the corresponding edge
// of the basis rectangle.  The offsets for the outer rectangle define how much to grow the edges of
// the basis rectangle to obtain the corresponding edges of the outer rectangle.  Similarly, the
// offsets for the inner rectangle define how much to shrink the edges of the basis rectangle to
// obtain the corresponding edges of the inner rectangle.  In this parlance, the outer rectangle
// offsets' default values are each 1, while the inner rectangle offsets' default values are each 0.
//
// The whole frame has a single texture applied to it, though each cell has particular texture
// coordinates assigned to it, so that each cell has a well-defined region of texture that is mapped
// to it.  The texture may stretch if the aspect ratio is not preserved, but this is by design.
// The default texture coordinates are given by defining the u and v (x and y) coordinates for each
// of the rectangle edges (labeled):
//
//   OUTER  INNER           INNER  OUTER
//   LEFT   LEFT            RIGHT  RIGHT
//
// v  0.0  0.25              0.75  1.0 
//     +---+-------------------+---+ 1.0    OUTER TOP
// ^   | 0 |         1         | 2 |
// |   +---+-------------------+---+ 0.75   INNER TOP
// |   |   |                   |   |
// |   | 3 |         4         | 5 |
// |   |   |                   |   |
// |   +---+-------------------+---+ 0.25   INNER BOTTOM
// |   | 6 |         7         | 8 |
//     +---+-------------------+---+ 0.0    OUTER BOTTOM
//
//      --------------->  u
//
// This choice is proportional to the default sizes of the cells, making the texture appear unstretched.
// The texture defaults to uninitialized (there is no natural default).  If there is no texture set
// when this primitive is drawn, then nothing is drawn.
class TexturedFrame : public PrimitiveBase {
public:
  
  enum class Rectangle : size_t { INNER = 0, OUTER };
  enum class RectangleEdge : size_t { TOP = 0, RIGHT, BOTTOM, LEFT };
  
  TexturedFrame ();
  virtual ~TexturedFrame();
  
  const Vector2 &BasisRectangleSize () const { return m_basis_rectangle_size; }
  double RectangleEdgeOffset (Rectangle rect, RectangleEdge edge) const { return m_rectangle_edge_offset[static_cast<size_t>(rect)][static_cast<size_t>(edge)]; }
  float RectangleEdgeTextureCoordinate (Rectangle rect, RectangleEdge edge) const { return m_rectangle_edge_texture_coordinate[static_cast<size_t>(rect)][static_cast<size_t>(edge)]; }
  const std::shared_ptr<GLTexture2> &Texture () const { return m_texture; }
  
  // The rectangle will be centered at the origin, and will extend half of each size component in each direction.
  void SetBasisRectangleSize (const Vector2 &size);
  // The inner (resp. outer) rectangles' edges will extend in (resp. out) from the basis rectangle by the given offset.
  void SetRectangleEdgeOffset (Rectangle rect, RectangleEdge edge, double offset);
  // Sets the texture coordinate for the given rectangle edge (see diagram for labeling).
  void SetRectangleEdgeTextureCoordinate (Rectangle rect, RectangleEdge edge, float tex_coord);
  // Set the texture for this primitive.  The texture coordinates will be unchanged.
  void SetTexture (const std::shared_ptr<GLTexture2> &texture) { m_texture = texture; }
  
protected:

  virtual void DrawContents (RenderState& renderState) const override;

private:
  
  void RecomputeGeometryIfNecessary () const;

  static const size_t RECTANGLE_COUNT = 2;
  static const size_t RECTANGLE_EDGE_COUNT = 4;
  static const size_t CELL_COUNT = 9;

  Vector2 m_basis_rectangle_size;
  double m_rectangle_edge_offset[RECTANGLE_COUNT][RECTANGLE_EDGE_COUNT];
  float m_rectangle_edge_texture_coordinate[RECTANGLE_COUNT][RECTANGLE_EDGE_COUNT];
  std::shared_ptr<GLTexture2> m_texture;
  mutable bool m_recompute_geometry;
  mutable PrimitiveGeometry m_geometry;
};
