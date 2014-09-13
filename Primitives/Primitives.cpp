#include "Primitives.h"

#include <cassert>
#include "GLTexture2.h"

void GenericShape::DrawContents(RenderState& renderState) const {
  m_geometry.Draw(Shader(), m_drawMode);
}

Sphere::Sphere() : m_Radius(1) { }

void Sphere::MakeAdditionalModelViewTransformations (ModelView &model_view) const {
  model_view.Scale(Vector3::Constant(m_Radius));
}

void Sphere::DrawContents(RenderState& renderState) const {
  static PrimitiveGeometry geom;
  static bool loaded = false;
  if (!loaded) {
    PrimitiveGeometry::CreateUnitSphere(30, geom);
    loaded = true;
  }
  geom.Draw(Shader(), GL_TRIANGLES);
}

Cylinder::Cylinder() : m_Radius(1), m_Height(1) { }

void Cylinder::MakeAdditionalModelViewTransformations (ModelView &model_view) const {
  model_view.Scale(Vector3(m_Radius, m_Height, m_Radius));
}

void Cylinder::DrawContents(RenderState& renderState) const {
  static PrimitiveGeometry geom;
  static bool loaded = false;
  if (!loaded) {
    PrimitiveGeometry::CreateUnitCylinder(50, 1, geom);
    loaded = true;
  }
  geom.Draw(Shader(), GL_TRIANGLES);
}

Box::Box() : m_Size(Vector3::Constant(1.0)) { }

void Box::MakeAdditionalModelViewTransformations (ModelView &model_view) const {
  model_view.Scale(m_Size);
}

void Box::DrawContents(RenderState& renderState) const {
  static PrimitiveGeometry geom;
  static bool loaded = false;
  if (!loaded) {
    PrimitiveGeometry::CreateUnitBox(geom);
    loaded = true;
  }
  geom.Draw(Shader(), GL_TRIANGLES);
}

Disk::Disk() : m_Radius(1) { }

void Disk::MakeAdditionalModelViewTransformations (ModelView &model_view) const {
  model_view.Scale(Vector3::Constant(m_Radius));
}

void Disk::DrawContents(RenderState& renderState) const {
  static PrimitiveGeometry geom;
  static bool loaded = false;
  if (!loaded) {
    PrimitiveGeometry::CreateUnitDisk(75, geom);
    loaded = true;
  }
  geom.Draw(Shader(), GL_TRIANGLES);
}

RectanglePrim::RectanglePrim() : m_Size(1, 1) { }

void RectanglePrim::MakeAdditionalModelViewTransformations (ModelView &model_view) const {
  model_view.Scale(Vector3(m_Size.x(), m_Size.y(), 1.0));
}

void RectanglePrim::DrawContents(RenderState& renderState) const {
  bool useTexture = bool(m_texture); // If there is a valid texture, enable texturing.
  if (useTexture) {
    glEnable(GL_TEXTURE_2D);
    m_texture->Bind();
  }
  static PrimitiveGeometry geom;
  static bool loaded = false;
  if (!loaded) {
    PrimitiveGeometry::CreateUnitSquare(geom);
    loaded = true;
  }
  geom.Draw(Shader(), GL_TRIANGLES);
  if (useTexture) {
    glDisable(GL_TEXTURE_2D);
    m_texture->Unbind();
  }
}

ImagePrimitive::ImagePrimitive(void) :
  ImagePrimitive(nullptr)
{}

ImagePrimitive::ImagePrimitive(const std::shared_ptr<GLTexture2> &texture) {
  SetTexture(texture);
  SetScaleBasedOnTextureSize();
  Material().SetAmbientLightingProportion(1.0f);
  Material().SetUseTexture(true);
}

void ImagePrimitive::SetScaleBasedOnTextureSize () {
  if(Texture())
    SetSize(Vector2(Texture()->Params().Width(), Texture()->Params().Height()));
}

PartialDisk::PartialDisk() : m_RecomputeGeometry(true), m_InnerRadius(0.5), m_OuterRadius(1), m_StartAngle(0), m_EndAngle(2*M_PI) { }

void PartialDisk::DrawContents(RenderState& renderState) const {
  if (m_InnerRadius >= m_OuterRadius || m_StartAngle >= m_EndAngle) {
    // don't proceed if the shape is empty
    return;
  }

  if (m_RecomputeGeometry) {
    RecomputeGeometry();
  }

  m_Geometry.Draw(Shader(), GL_TRIANGLES);
}

void PartialDisk::RecomputeGeometry() const {
  double sweepAngle = m_EndAngle - m_StartAngle;
  if (sweepAngle > 2*M_PI) {
    sweepAngle = 2*M_PI;
  }

  static const double DESIRED_ANGLE_PER_SEGMENT = 0.1; // radians
  const int numSegments = static_cast<int>(sweepAngle / DESIRED_ANGLE_PER_SEGMENT) + 1;
  const double anglePerSegment = sweepAngle / numSegments;

  m_Geometry.CleanUpBuffers();

  double curAngle = m_StartAngle;
  const double cosStart = std::cos(m_StartAngle);
  const double sinStart = std::sin(m_StartAngle);
  Vector3f prevInner(static_cast<float>(m_InnerRadius*cosStart), static_cast<float>(m_InnerRadius*sinStart), 0.0f);
  Vector3f prevOuter(static_cast<float>(m_OuterRadius*cosStart), static_cast<float>(m_OuterRadius*sinStart), 0.0f);
  for (int i=0; i<numSegments; i++) {
    curAngle += anglePerSegment;

    const double cosCur = std::cos(curAngle);
    const double sinCur = std::sin(curAngle);

    const Vector3f curInner(static_cast<float>(m_InnerRadius*cosCur), static_cast<float>(m_InnerRadius*sinCur), 0.0f);
    const Vector3f curOuter(static_cast<float>(m_OuterRadius*cosCur), static_cast<float>(m_OuterRadius*sinCur), 0.0f);

    m_Geometry.PushTri(prevInner, prevOuter, curOuter);
    m_Geometry.PushTri(curOuter, curInner, prevInner);

    prevInner = curInner;
    prevOuter = curOuter;
  }

  m_Geometry.UploadDataToBuffers();

  m_RecomputeGeometry = false;
}

PartialDiskWithTriangle::PartialDiskWithTriangle()
  :
  m_TriangleSide(OUTSIDE),
  m_TrianglePosition(0.5),
  m_TriangleWidth(0.1),
  m_TriangleOffset(0.35)
{ }

void PartialDiskWithTriangle::RecomputeGeometry() const {
  double sweepAngle = m_EndAngle - m_StartAngle;
  if (sweepAngle > 2*M_PI) {
    sweepAngle = 2*M_PI;
  }

  static const double DESIRED_ANGLE_PER_SEGMENT = 0.1; // radians
  int numSegments = static_cast<int>(sweepAngle / DESIRED_ANGLE_PER_SEGMENT) + 1;
  const double anglePerSegment = sweepAngle / numSegments;

  m_Geometry.CleanUpBuffers();

  double curAngle = m_StartAngle;
  const double cosStart = std::cos(m_StartAngle);
  const double sinStart = std::sin(m_StartAngle);
  Vector3f prevInner(static_cast<float>(m_InnerRadius*cosStart), static_cast<float>(m_InnerRadius*sinStart), 0.0f);
  Vector3f prevOuter(static_cast<float>(m_OuterRadius*cosStart), static_cast<float>(m_OuterRadius*sinStart), 0.0f);

  bool haveStarted = false;
  bool havePassedMidpoint = false;
  bool havePassedEnd = false;
  bool haveTakenCareOfExtraAngle = false;

  const double triangleAngle = sweepAngle * m_TriangleWidth;
  const double triangleStart = m_TrianglePosition * sweepAngle + m_StartAngle - triangleAngle / 2.0;
  const double triangleEnd = triangleStart + triangleAngle;
  const double triangleMidpoint = 0.5*(triangleStart + triangleEnd);

  while (curAngle < (m_EndAngle - 0.001)) {
    curAngle += anglePerSegment;

    if (!haveStarted && curAngle > triangleStart) {
      curAngle = triangleStart;
      haveStarted = true;
    } else if (!havePassedMidpoint && curAngle > triangleMidpoint) {
      curAngle = triangleMidpoint;
      havePassedMidpoint = true;
    } else if (!havePassedEnd && curAngle > triangleEnd) {
      curAngle = triangleEnd;
      havePassedEnd = true;
    } else if (havePassedEnd && !haveTakenCareOfExtraAngle) {
      haveTakenCareOfExtraAngle = true;
      curAngle = m_StartAngle + anglePerSegment * (static_cast<int>((curAngle-m_StartAngle) / anglePerSegment));
    }

    double innerRadius = m_InnerRadius;
    double outerRadius = m_OuterRadius;
    if (curAngle >= triangleStart && curAngle <= triangleEnd) {
      double ratio = (curAngle - triangleStart) / (triangleAngle);
      double mult = -2 * std::abs(ratio-0.5) + 1;
      const double triangleHeight = m_TriangleOffset * (m_OuterRadius - m_InnerRadius);
      if (m_TriangleSide == INSIDE) {
        innerRadius -= mult * triangleHeight;
      } else if (m_TriangleSide == OUTSIDE) {
        outerRadius += mult * triangleHeight;
      }
    }

    const double cosCur = std::cos(curAngle);
    const double sinCur = std::sin(curAngle);

    const Vector3f curInner(static_cast<float>(innerRadius*cosCur), static_cast<float>(innerRadius*sinCur), 0.0f);
    const Vector3f curOuter(static_cast<float>(outerRadius*cosCur), static_cast<float>(outerRadius*sinCur), 0.0f);

    m_Geometry.PushTri(prevInner, prevOuter, curOuter);
    m_Geometry.PushTri(curOuter, curInner, prevInner);

    prevInner = curInner;
    prevOuter = curOuter;
  }

  m_Geometry.UploadDataToBuffers();

  m_RecomputeGeometry = false;
}

TexturedFrame::TexturedFrame() {
  SetBasisRectangleSize(Vector2(1.0, 1.0));

  // Offsets for inner rectangle.
  SetRectangleEdgeOffset(Rectangle::INNER, RectangleEdge::TOP,    0.0);
  SetRectangleEdgeOffset(Rectangle::INNER, RectangleEdge::RIGHT,  0.0);
  SetRectangleEdgeOffset(Rectangle::INNER, RectangleEdge::BOTTOM, 0.0);
  SetRectangleEdgeOffset(Rectangle::INNER, RectangleEdge::LEFT,   0.0);
  
  // Offsets for inner rectangle.
  SetRectangleEdgeOffset(Rectangle::OUTER, RectangleEdge::TOP,    1.0);
  SetRectangleEdgeOffset(Rectangle::OUTER, RectangleEdge::RIGHT,  1.0);
  SetRectangleEdgeOffset(Rectangle::OUTER, RectangleEdge::BOTTOM, 1.0);
  SetRectangleEdgeOffset(Rectangle::OUTER, RectangleEdge::LEFT,   1.0);
  
  // Vertical edges' texture coordinates.
  SetRectangleEdgeTextureCoordinate(Rectangle::OUTER,  RectangleEdge::LEFT, 0.0f);
  SetRectangleEdgeTextureCoordinate(Rectangle::INNER,  RectangleEdge::LEFT, 0.25f);
  SetRectangleEdgeTextureCoordinate(Rectangle::INNER, RectangleEdge::RIGHT, 0.75f);
  SetRectangleEdgeTextureCoordinate(Rectangle::OUTER, RectangleEdge::RIGHT, 1.0f);

  // Horizontal edges' texture coordinates.
  SetRectangleEdgeTextureCoordinate(Rectangle::OUTER, RectangleEdge::BOTTOM, 0.0f);
  SetRectangleEdgeTextureCoordinate(Rectangle::INNER, RectangleEdge::BOTTOM, 0.25f);
  SetRectangleEdgeTextureCoordinate(Rectangle::INNER,    RectangleEdge::TOP, 0.75f);
  SetRectangleEdgeTextureCoordinate(Rectangle::OUTER,    RectangleEdge::TOP, 1.0f);

  // Set up the material properties regarding textures
  Material().SetUseTexture(true);
  
  m_recompute_geometry = true;
}

TexturedFrame::~TexturedFrame() { }

void TexturedFrame::SetBasisRectangleSize(const Vector2& size) {
  m_basis_rectangle_size = size;
  m_recompute_geometry = true;
}

void TexturedFrame::SetRectangleEdgeOffset(TexturedFrame::Rectangle rect, TexturedFrame::RectangleEdge edge, double offset) {
  double &o = m_rectangle_edge_offset[size_t(rect)][size_t(edge)];
  offset = std::max(0.0, offset);
  if (o != offset) {
    o = offset;
    m_recompute_geometry = true;
  }
}

void TexturedFrame::SetRectangleEdgeTextureCoordinate(TexturedFrame::Rectangle rect, TexturedFrame::RectangleEdge edge, GLfloat tex_coord) {
  GLfloat &tc = m_rectangle_edge_texture_coordinate[size_t(rect)][size_t(edge)];
  if (tc != tex_coord) {
    tc = tex_coord;
    m_recompute_geometry = true;
  }
}

void TexturedFrame::DrawContents(RenderState& renderState) const {
  if (!m_texture) {
    return; // If the texture is not set, don't draw anything.
  }
  
  RecomputeGeometryIfNecessary();
  assert(!m_recompute_geometry);

  glEnable(GL_TEXTURE_2D);
  m_texture->Bind();
  m_geometry.Draw(Shader(), GL_TRIANGLES);
  m_texture->Unbind();
  glDisable(GL_TEXTURE_2D);
  
  // m_geometry.Draw(Shader(), GL_LINE_STRIP);
}

void TexturedFrame::RecomputeGeometryIfNecessary() const {
  if (!m_recompute_geometry) {
    return;
  }
  
  m_geometry.CleanUpBuffers();
  
  const double bx = 0.5 * m_basis_rectangle_size(0);
  const double by = 0.5 * m_basis_rectangle_size(1);
  // The first index indicates x (0) or y (1).
  const double rectangle_edge[2][4]{
    {
      -bx - RectangleEdgeOffset(Rectangle::OUTER, RectangleEdge::LEFT),
      -bx + RectangleEdgeOffset(Rectangle::INNER, RectangleEdge::LEFT),
       bx - RectangleEdgeOffset(Rectangle::INNER, RectangleEdge::RIGHT),
       bx + RectangleEdgeOffset(Rectangle::OUTER, RectangleEdge::RIGHT)
    },
    {
      -by - RectangleEdgeOffset(Rectangle::OUTER, RectangleEdge::BOTTOM),
      -by + RectangleEdgeOffset(Rectangle::INNER, RectangleEdge::BOTTOM),
       by - RectangleEdgeOffset(Rectangle::INNER, RectangleEdge::TOP),
       by + RectangleEdgeOffset(Rectangle::OUTER, RectangleEdge::TOP)
    }
  };
  const float rectangle_edge_texture_coordinate[2][4]{
    {
      RectangleEdgeTextureCoordinate(Rectangle::OUTER, RectangleEdge::LEFT),
      RectangleEdgeTextureCoordinate(Rectangle::INNER, RectangleEdge::LEFT),
      RectangleEdgeTextureCoordinate(Rectangle::INNER, RectangleEdge::RIGHT),
      RectangleEdgeTextureCoordinate(Rectangle::OUTER, RectangleEdge::RIGHT),
    },
    {
      RectangleEdgeTextureCoordinate(Rectangle::OUTER, RectangleEdge::BOTTOM),
      RectangleEdgeTextureCoordinate(Rectangle::INNER, RectangleEdge::BOTTOM),
      RectangleEdgeTextureCoordinate(Rectangle::INNER, RectangleEdge::TOP),
      RectangleEdgeTextureCoordinate(Rectangle::OUTER, RectangleEdge::TOP),
    }
  };

  static const Vector3f NORMAL = Vector3f::UnitZ();
  static const Vector4f COLOR = Vector4f::Constant(1.0f);
  // The spatial layout of the vertices is:
  //    [0][3]    [1][3]    [2][3]    [3][3]
  //    [0][2]    [1][2]    [2][2]    [3][2]
  //    [0][1]    [1][1]    [2][1]    [3][1]
  //    [0][0]    [1][0]    [2][0]    [3][0]
  PrimitiveGeometry::VertexAttributes vertex_attributes[4][4];
  for (size_t u = 0; u < 4; ++u) {
    for (size_t v = 0; v < 4; ++v) {
      vertex_attributes[u][v] = std::make_tuple(Vector3f(rectangle_edge[0][u], rectangle_edge[1][v], 0.0f),
                                                NORMAL,
                                                Vector2f(rectangle_edge_texture_coordinate[0][u], rectangle_edge_texture_coordinate[1][v]),
                                                COLOR);
    }
  }

  for (size_t u = 0; u < 3; ++u) {
    for (size_t v = 0; v < 3; ++v) {
      m_geometry.PushQuad(vertex_attributes[u+0][v+0],
                          vertex_attributes[u+1][v+0],
                          vertex_attributes[u+1][v+1],
                          vertex_attributes[u+0][v+1]);
    }
  }

  m_geometry.UploadDataToBuffers();

  m_recompute_geometry = false;
}
