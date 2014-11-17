#include "PrimitiveGeometry.h"

#include <iostream> // TEMP

#include "GLShader.h"

PrimitiveGeometry::PrimitiveGeometry()
  :
  m_VertexBuffer(GL_STATIC_DRAW),
  m_NumIndices(0)
{ }

void PrimitiveGeometry::CleanUpBuffers() {
  m_Vertices.clear();
  m_VertexBuffer.ClearEverything();
  m_NumIndices = 0;
  if (m_IndexBuffer.IsCreated()) {
    m_IndexBuffer.Destroy();
  }
}

void PrimitiveGeometry::UploadDataToBuffers(ClearOption clear_option) {
  // Eliminate duplicate vertices using a temporary map.
  VertexIndexMap vertex_attributes_index_map;
  std::vector<GLuint> indices;
  // std::vector<VertexAttributes> unique_vertex_attributes;
  std::vector<VertexAttributes> &unique_vertex_attributes = m_VertexBuffer.IntermediateAttributes();
  // This loop will reduce m_Vertices down into unique_vertex_attributes.
  for (auto it = m_Vertices.begin(); it != m_Vertices.end(); ++it) {
    const VertexAttributes &vertex_attributes = *it;
    auto mapped_vertex_attribute = vertex_attributes_index_map.find(vertex_attributes);
    // If the current vertex is not in the vbo map already, add it.
    if (mapped_vertex_attribute == vertex_attributes_index_map.end()) {
      GLuint new_index = static_cast<GLuint>(vertex_attributes_index_map.size());
      indices.push_back(new_index);
      unique_vertex_attributes.push_back(vertex_attributes);
      vertex_attributes_index_map[vertex_attributes] = new_index;
    } else { // Otherwise, add the existing vertex's index.
      indices.push_back(mapped_vertex_attribute->second);
    }
  }

  if (clear_option == ClearOption::CLEAR_INTERMEDIATE_DATA) {
    m_Vertices.clear();
  }

  m_VertexBuffer.UploadIntermediateAttributes();

  m_NumIndices = static_cast<int>(indices.size());

  m_IndexBuffer.Create(GL_ELEMENT_ARRAY_BUFFER);
  m_IndexBuffer.Bind();
  m_IndexBuffer.Allocate(static_cast<void*>(indices.data()), static_cast<int>(indices.size()*sizeof(unsigned int)), GL_STATIC_DRAW);
  m_IndexBuffer.Unbind();
  
  if (clear_option == ClearOption::CLEAR_INTERMEDIATE_DATA) {
    m_VertexBuffer.ClearIntermediateAttributes();
  }
}

void PrimitiveGeometry::Draw(const GLShader &bound_shader, GLenum drawMode) const {
  auto locations = std::make_tuple(bound_shader.LocationOfAttribute("position"),
                                   bound_shader.LocationOfAttribute("normal"),
                                   bound_shader.LocationOfAttribute("tex_coord"),
                                   bound_shader.LocationOfAttribute("color"));
  // This calls glEnableVertexAttribArray and glVertexAttribPointer on the relevant things.
  m_VertexBuffer.Enable(locations);

  m_IndexBuffer.Bind();
  GL_THROW_UPON_ERROR(glDrawElements(drawMode, m_NumIndices, GL_UNSIGNED_INT, 0));
  m_IndexBuffer.Unbind();

  // This calls glDisableVertexAttribArray on the relevant things.
  m_VertexBuffer.Disable(locations);
}

void PrimitiveGeometry::CreateUnitSphere(int widthResolution, int heightResolution, PrimitiveGeometry& geom, double heightAngleStart, double heightAngleEnd, double widthAngleStart, double widthAngleEnd) {
  const float resFloatW = static_cast<float>(widthResolution);
  const float resFloatH = static_cast<float>(heightResolution);
  const float pi = static_cast<float>(M_PI);
  const float twoPi = static_cast<float>(2.0 * M_PI);

  const float widthStart = static_cast<float>(widthAngleStart);
  const float widthSweep = std::min(twoPi, static_cast<float>(widthAngleEnd - widthAngleStart));
  const float heightStart = static_cast<float>(heightAngleStart);
  const float heightSweep = std::min(pi, static_cast<float>(heightAngleEnd - heightAngleStart));

  for (int w=0; w<widthResolution; w++) {
    for (int h=0; h<heightResolution; h++) {
      const float inc1 = (w/resFloatW) * widthSweep + widthStart;
      const float inc2 = ((w+1)/resFloatW) * widthSweep + widthStart;
      const float inc3 = (h/resFloatH) * heightSweep + heightStart;
      const float inc4 = ((h+1)/resFloatH) * heightSweep + heightStart;

      // compute first position
      const float x1 = std::sin(inc1);
      const float y1 = std::cos(inc1);
      const float z1 = std::sin(inc3);

      // compute second position
      const float x2 = std::sin(inc2);
      const float y2 = std::cos(inc2);
      const float z2 = std::sin(inc4);

      // compute radii at these positions
      const float r1 = std::cos(inc3);
      const float r2 = std::cos(inc4);

      // form triangles and compute their vertices
      const EigenTypes::Vector3f v1(r1*x1, z1, r1*y1);
      const EigenTypes::Vector3f v2(r1*x2, z1, r1*y2);
      const EigenTypes::Vector3f v3(r2*x2, z2, r2*y2);
      const EigenTypes::Vector3f v4(r1*x1, z1, r1*y1);
      const EigenTypes::Vector3f v5(r2*x2, z2, r2*y2);
      const EigenTypes::Vector3f v6(r2*x1, z2, r2*y1);

      auto SphereVertex = [](const EigenTypes::Vector3f &v){ return MakeVertexAttributes(v, v.normalized()); };
      geom.PushTri(SphereVertex(v1), SphereVertex(v2), SphereVertex(v3));
      geom.PushTri(SphereVertex(v4), SphereVertex(v5), SphereVertex(v6));
    }
  }

  geom.UploadDataToBuffers();
}

void PrimitiveGeometry::CreateUnitCylinder(int radialResolution, int verticalResolution, PrimitiveGeometry& geom, float radiusBottom, float radiusTop, double angleStart, double angleEnd) {
  const float radialRes = 1.0f / static_cast<float>(radialResolution);
  const float verticalRes = 1.0f / static_cast<float>(verticalResolution);
  
  const float twoPi = static_cast<float>(2.0 * M_PI);
  const float start = static_cast<float>(angleStart);
  const float sweep = std::min(twoPi, static_cast<float>(angleEnd - angleStart));

  for (int w=0; w<radialResolution; w++) {
    const float inc1 = w * radialRes * sweep + start;
    const float inc2 = (w+1) * radialRes * sweep + start;

    const float c1 = std::cos(inc1);
    const float c2 = std::cos(inc2);
    const float s1 = std::sin(inc1);
    const float s2 = std::sin(inc2);

    // vectors perpendicular from center axis to wall
    const EigenTypes::Vector3f p1(c1, 0, s1);
    const EigenTypes::Vector3f p2(c2, 0, s2);

    for (int h=0; h<verticalResolution; h++) {
      const float ratio1 = h*verticalRes;
      const float ratio2 = (h+1)*verticalRes;

      const float h1 = ratio1 - 0.5f;
      const float h2 = ratio2 - 0.5f;

      const float r1 = (1.0f-ratio1)*radiusBottom + ratio1*radiusTop;
      const float r2 = (1.0f-ratio2)*radiusBottom + ratio2*radiusTop;

      const EigenTypes::Vector3f v1(r1*c1, h1, r1*s1);
      const EigenTypes::Vector3f v2(r2*c1, h2, r2*s1);
      const EigenTypes::Vector3f v3(r1*c2, h1, r1*s2);
      const EigenTypes::Vector3f v4(r2*c2, h2, r2*s2);

      // tangents
      const EigenTypes::Vector3f t1 = v2 - v1;
      const EigenTypes::Vector3f t2 = v4 - v3;

      // binormals
      const EigenTypes::Vector3f b1 = (p1.cross(t1));
      const EigenTypes::Vector3f b2 = (p2.cross(t2));

      // normals
      const EigenTypes::Vector3f n1 = t1.cross(b1).normalized();
      const EigenTypes::Vector3f n2 = t2.cross(b2).normalized();

      geom.PushTri(MakeVertexAttributes(v1, n1), MakeVertexAttributes(v2, n1), MakeVertexAttributes(v3, n2));
      geom.PushTri(MakeVertexAttributes(v4, n2), MakeVertexAttributes(v3, n2), MakeVertexAttributes(v2, n1));
    }
  }

  geom.UploadDataToBuffers();
}

void PrimitiveGeometry::CreateUnitSquare(PrimitiveGeometry& geom) {
  static const GLfloat X = 0.5f;
  static const EigenTypes::Vector3f POSITIONS[4] = {
    EigenTypes::Vector3f(-X, -X, 0),
    EigenTypes::Vector3f( X, -X, 0),
    EigenTypes::Vector3f( X,  X, 0),
    EigenTypes::Vector3f(-X,  X, 0)
  };
  static const EigenTypes::Vector2f TEX_COORDS[4] = {
    EigenTypes::Vector2f(0, 0),
    EigenTypes::Vector2f(1, 0),
    EigenTypes::Vector2f(1, 1),
    EigenTypes::Vector2f(0, 1)
  };
  
  // all vertices have the same normal
  const EigenTypes::Vector3f normal(EigenTypes::Vector3f::UnitZ());

  geom.PushTri(MakeVertexAttributes(POSITIONS[0], normal, TEX_COORDS[0]),
               MakeVertexAttributes(POSITIONS[1], normal, TEX_COORDS[1]),
               MakeVertexAttributes(POSITIONS[2], normal, TEX_COORDS[2]));
  geom.PushTri(MakeVertexAttributes(POSITIONS[0], normal, TEX_COORDS[0]),
               MakeVertexAttributes(POSITIONS[2], normal, TEX_COORDS[2]),
               MakeVertexAttributes(POSITIONS[3], normal, TEX_COORDS[3]));

  geom.UploadDataToBuffers();
}

void PrimitiveGeometry::CreateUnitDisk(int resolution, PrimitiveGeometry& geom) {
  const EigenTypes::Vector3f center(EigenTypes::Vector3f::Zero());

  const float resFloat = static_cast<float>(resolution);
  const float twoPi = static_cast<float>(2.0 * M_PI);

  for (int i=0; i<resolution; i++) {
    const float inc1 = (i/resFloat) * twoPi;
    const float inc2 = ((i+1)/resFloat) * twoPi;

    const float c1 = std::cos(inc1);
    const float c2 = std::cos(inc2);
    const float s1 = std::sin(inc1);
    const float s2 = std::sin(inc2);

    const EigenTypes::Vector3f p1(c1, s1, 0.0f);
    const EigenTypes::Vector3f p2(c2, s2, 0.0f);

    geom.PushTri(center, p1, p2);
  }

  geom.UploadDataToBuffers();
}

void PrimitiveGeometry::CreateUnitBox(PrimitiveGeometry& geom) {
  // In order for this to be a unit box, its side lengths must be unit.
  const float x = 0.5f;
  const EigenTypes::Vector3f p000(-x, -x, -x);
  const EigenTypes::Vector3f p001(-x, -x,  x);
  const EigenTypes::Vector3f p010(-x,  x, -x);
  const EigenTypes::Vector3f p011(-x,  x,  x);
  const EigenTypes::Vector3f p100( x, -x, -x);
  const EigenTypes::Vector3f p101( x, -x,  x);
  const EigenTypes::Vector3f p110( x,  x, -x);
  const EigenTypes::Vector3f p111( x,  x,  x);

  geom.PushQuad(p010, p000, p001, p011);
  geom.PushQuad(p100, p110, p111, p101);
  geom.PushQuad(p000, p100, p101, p001);
  geom.PushQuad(p110, p010, p011, p111);
  geom.PushQuad(p010, p110, p100, p000);
  geom.PushQuad(p001, p101, p111, p011);
  
  geom.UploadDataToBuffers();
}

void PrimitiveGeometry::PushTri(const VertexAttributes& p0, const VertexAttributes& p1, const VertexAttributes& p2) {
  // The orientation is given by the counterclockwise traversal of the points using the right-hand rule.
  m_Vertices.push_back(p0);
  m_Vertices.push_back(p1);
  m_Vertices.push_back(p2);
}

void PrimitiveGeometry::PushTri(const EigenTypes::Vector3f& p0, const EigenTypes::Vector3f& p1, const EigenTypes::Vector3f& p2) {
  EigenTypes::Vector3f normal((p2-p1).cross(p0-p1).normalized());
  PushTri(MakeVertexAttributes(p0, normal), MakeVertexAttributes(p1, normal), MakeVertexAttributes(p2, normal));
}

void PrimitiveGeometry::PushQuad (const VertexAttributes &p0, const VertexAttributes &p1, const VertexAttributes &p2, const VertexAttributes &p3) {
  // The orientation is given by the counterclockwise traversal of the points using the right-hand rule.
  m_Vertices.push_back(p0);
  m_Vertices.push_back(p1);
  m_Vertices.push_back(p2);
  
  m_Vertices.push_back(p0);
  m_Vertices.push_back(p2);
  m_Vertices.push_back(p3);
}

void PrimitiveGeometry::PushQuad(const EigenTypes::Vector3f& p0, const EigenTypes::Vector3f& p1, const EigenTypes::Vector3f& p2, const EigenTypes::Vector3f& p3) {
  // The orientation is given by the counterclockwise traversal of the points using the right-hand rule.

  static const EigenTypes::Vector2f TEX_COORDS[4] = {
    EigenTypes::Vector2f(0.0f, 0.0f),
    EigenTypes::Vector2f(1.0f, 0.0f),
    EigenTypes::Vector2f(1.0f, 1.0f),
    EigenTypes::Vector2f(0.0f, 1.0f)
  };

  // Compute the normal for each triangle, where the quad is split up into 2 triangles, having its diagonal between points p0 and p2.
  EigenTypes::Vector3f n0((p2-p1).cross(p0-p1).normalized());
  EigenTypes::Vector3f n1((p3-p2).cross(p0-p2).normalized());
  
  PushTri(MakeVertexAttributes(p0, n0, TEX_COORDS[0]), MakeVertexAttributes(p1, n0, TEX_COORDS[1]), MakeVertexAttributes(p2, n0, TEX_COORDS[2]));
  PushTri(MakeVertexAttributes(p0, n1, TEX_COORDS[0]), MakeVertexAttributes(p2, n1, TEX_COORDS[2]), MakeVertexAttributes(p3, n1, TEX_COORDS[3]));
}
