#include "PrimitiveGeometry.h"

#include <iostream> // TEMP

#include "GLShader.h"

PrimitiveGeometry::PrimitiveGeometry() : m_NumIndices(0) { }

void PrimitiveGeometry::CleanUpBuffers() {
  m_Vertices.clear();
  if (m_VertexBuffer.IsCreated()) {
    m_VertexBuffer.Destroy();
  }
  m_NumIndices = 0;
  if (m_IndexBuffer.IsCreated()) {
    m_IndexBuffer.Destroy();
  }
}

void PrimitiveGeometry::UploadDataToBuffers(ClearOption clear_option) {
  // Eliminate duplicate vertices using a temporary map.
  VertexIndexMap vertex_index_map;
  std::vector<unsigned int> indices;
  std::vector<Vertex> unique_vertices;
  // This loop will reduce m_Vertices down into unique_vertices.
  for (auto it = m_Vertices.begin(); it != m_Vertices.end(); ++it) {
    const Vertex &vertex = *it;
    auto mapped_vertex = vertex_index_map.find(vertex);
    // If the current vertex is not in the vbo map already, add it.
    if (mapped_vertex == vertex_index_map.end()) {
      unsigned int new_index = vertex_index_map.size();
      indices.push_back(new_index);
      unique_vertices.push_back(vertex);
      vertex_index_map[vertex] = new_index;
    } else { // Otherwise, add the existing vertex's index.
      indices.push_back(mapped_vertex->second);
    }
  }

  m_VertexBuffer.Create(GL_ARRAY_BUFFER);
  m_VertexBuffer.Bind();
  m_VertexBuffer.Allocate(static_cast<void*>(unique_vertices.data()), static_cast<int>(unique_vertices.size()*sizeof(Vertex)), GL_STATIC_DRAW);
  m_VertexBuffer.Unbind();

  m_NumIndices = static_cast<int>(indices.size());

  m_IndexBuffer.Create(GL_ELEMENT_ARRAY_BUFFER);
  m_IndexBuffer.Bind();
  m_IndexBuffer.Allocate(static_cast<void*>(indices.data()), static_cast<int>(indices.size()*sizeof(unsigned int)), GL_STATIC_DRAW);
  m_IndexBuffer.Unbind();
  
  if (clear_option == ClearOption::CLEAR_INTERMEDIATE_DATA) {
    m_Vertices.clear();
  }
}

void PrimitiveGeometry::Draw(const GLShader &bound_shader, GLenum drawMode) const {
  // Each attribute is optional.  Only enable and upload attributes that are enabled
  // (that's what the check for xxx_loc != -1 is).
  
  GLint position_loc = bound_shader.LocationOfAttribute("position");
  GLint normal_loc = bound_shader.LocationOfAttribute("normal");
  GLint tex_coord_loc = bound_shader.LocationOfAttribute("tex_coord");
  GLint color_loc = bound_shader.LocationOfAttribute("color");

  // The calls to glVertexAttribPointer use the currently bound buffer object.
  // NOTE: There being no "else { glDisableVertexAttribArray(...); }" statement
  // relies on each vertex attrib array being disabled to begin with.
  m_VertexBuffer.Bind();
  if (position_loc != -1) {
    glEnableVertexAttribArray(position_loc);
    glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(Vertex::OffsetOfPositionAttribute()));
  }
  if (normal_loc != -1) {
    glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(Vertex::OffsetOfPositionAttribute()));
    glEnableVertexAttribArray(normal_loc);
  }
  if (tex_coord_loc != -1) {
    glVertexAttribPointer(tex_coord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(Vertex::OffsetOfTexCoordAttribute()));
    glEnableVertexAttribArray(tex_coord_loc);
  }
  if (color_loc != -1) {
    glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(Vertex::OffsetOfColorAttribute()));
    glEnableVertexAttribArray(color_loc);
  }
  m_VertexBuffer.Unbind();
  
  m_IndexBuffer.Bind();
  glDrawElements(drawMode, m_NumIndices, GL_UNSIGNED_INT, 0);
  m_IndexBuffer.Unbind();

  // Disable the vertex attrib arrays that were enabled.
  if (position_loc != -1) {
    glDisableVertexAttribArray(position_loc);
  }
  if (normal_loc != -1) {
    glDisableVertexAttribArray(normal_loc);
  }
  if (tex_coord_loc != -1) {
    glDisableVertexAttribArray(tex_coord_loc);
  }
  if (color_loc != -1) {
    glDisableVertexAttribArray(color_loc);
  }
}

PrimitiveGeometry PrimitiveGeometry::CreateUnitSphere(int resolution) {
  PrimitiveGeometry geom;

  const float resFloat = static_cast<float>(resolution);
  const float pi = static_cast<float>(M_PI);
  const float twoPi = static_cast<float>(2.0 * M_PI);

  for (int w=0; w<resolution; w++) {
    for (int h=-resolution/2; h<resolution/2; h++) {
      const float inc1 = (w/resFloat) * twoPi;
      const float inc2 = ((w+1)/resFloat) * twoPi;
      const float inc3 = (h/resFloat) * pi;
      const float inc4 = ((h+1)/resFloat) * pi;

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
      const Vector3f v1(r1*x1, z1, r1*y1);
      const Vector3f v2(r1*x2, z1, r1*y2);
      const Vector3f v3(r2*x2, z2, r2*y2);
      const Vector3f v4(r1*x1, z1, r1*y1);
      const Vector3f v5(r2*x2, z2, r2*y2);
      const Vector3f v6(r2*x1, z2, r2*y1);

      auto SphereVertex = [](const Vector3f &v){ return Vertex(v, v.normalized()); };
      geom.PushTri(SphereVertex(v1), SphereVertex(v2), SphereVertex(v3));
      geom.PushTri(SphereVertex(v4), SphereVertex(v5), SphereVertex(v6));
    }
  }

  geom.UploadDataToBuffers();
  return geom;
}

PrimitiveGeometry PrimitiveGeometry::CreateUnitCylinder(int radialResolution, int verticalResolution) {
  PrimitiveGeometry geom;

  const float radialRes = 1.0f / static_cast<float>(radialResolution);
  const float verticalRes = 1.0f / static_cast<float>(verticalResolution);
  
  const float twoPi = static_cast<float>(2.0 * M_PI);

  for (int w=0; w<radialResolution; w++) {
    const float inc1 = w * radialRes * twoPi;
    const float inc2 = (w+1) * radialRes * twoPi;

    const float c1 = std::cos(inc1);
    const float c2 = std::cos(inc2);
    const float s1 = std::sin(inc1);
    const float s2 = std::sin(inc2);

    for (int h=0; h<verticalResolution; h++) {
      const float h1 = h * verticalRes - 0.5f;
      const float h2 = (h+1) * verticalRes - 0.5f;

      const Vector3f v1(c1, h1, s1);
      const Vector3f v2(c1, h2, s1);
      const Vector3f v3(c2, h1, s2);
      const Vector3f v4(c2, h2, s2);

      const Vector3f n1(c1, 0, s1);
      const Vector3f n2(c2, 0, s2);

      geom.PushTri(Vertex(v1, n1), Vertex(v2, n1), Vertex(v3, n2));
      geom.PushTri(Vertex(v4, n2), Vertex(v3, n2), Vertex(v2, n1));
    }
  }

  geom.UploadDataToBuffers();
  return geom;
}

PrimitiveGeometry PrimitiveGeometry::CreateUnitSquare() {
  PrimitiveGeometry geom;

  static const GLfloat X = 0.5f;
  static const Vector3f POSITIONS[4] = {
    Vector3f(-X, -X, 0),
    Vector3f( X, -X, 0),
    Vector3f( X,  X, 0),
    Vector3f(-X,  X, 0)
  };
  static const Vector2f TEX_COORDS[4] = {
    Vector2f(0, 0),
    Vector2f(1, 0),
    Vector2f(1, 1),
    Vector2f(0, 1)
  };
  
  // all vertices have the same normal
  const Vector3f normal(Vector3f::UnitZ());

  geom.PushTri(Vertex(POSITIONS[0], normal, TEX_COORDS[0]),
               Vertex(POSITIONS[1], normal, TEX_COORDS[1]),
               Vertex(POSITIONS[2], normal, TEX_COORDS[2]));
  geom.PushTri(Vertex(POSITIONS[0], normal, TEX_COORDS[0]),
               Vertex(POSITIONS[2], normal, TEX_COORDS[2]),
               Vertex(POSITIONS[3], normal, TEX_COORDS[3]));

  geom.UploadDataToBuffers();
  return geom;
}

PrimitiveGeometry PrimitiveGeometry::CreateUnitDisk(int resolution) {
  PrimitiveGeometry geom;

  const Vector3f center(Vector3f::Zero());

  const float resFloat = static_cast<float>(resolution);
  const float twoPi = static_cast<float>(2.0 * M_PI);

  for (int i=0; i<resolution; i++) {
    const float inc1 = (i/resFloat) * twoPi;
    const float inc2 = ((i+1)/resFloat) * twoPi;

    const float c1 = std::cos(inc1);
    const float c2 = std::cos(inc2);
    const float s1 = std::sin(inc1);
    const float s2 = std::sin(inc2);

    const Vector3f p1(c1, s1, 0.0f);
    const Vector3f p2(c2, s2, 0.0f);

    geom.PushTri(center, p1, p2);
  }

  geom.UploadDataToBuffers();
  return geom;
}

PrimitiveGeometry PrimitiveGeometry::CreateUnitBox() {
  PrimitiveGeometry geom;

  // In order for this to be a unit box, its side lengths must be unit.
  const float x = 0.5f;
  const Vector3f p000(-x, -x, -x);
  const Vector3f p001(-x, -x,  x);
  const Vector3f p010(-x,  x, -x);
  const Vector3f p011(-x,  x,  x);
  const Vector3f p100( x, -x, -x);
  const Vector3f p101( x, -x,  x);
  const Vector3f p110( x,  x, -x);
  const Vector3f p111( x,  x,  x);

  geom.PushQuad(p010, p000, p001, p011);
  geom.PushQuad(p100, p110, p111, p101);
  geom.PushQuad(p000, p100, p101, p001);
  geom.PushQuad(p110, p010, p011, p111);
  geom.PushQuad(p010, p110, p100, p000);
  geom.PushQuad(p001, p101, p111, p011);
  
  geom.UploadDataToBuffers();
  return geom;
}

void PrimitiveGeometry::PushTri(const Vertex& p0, const Vertex& p1, const Vertex& p2) {
  // The orientation is given by the counterclockwise traversal of the points using the right-hand rule.
  m_Vertices.push_back(p0);
  m_Vertices.push_back(p1);
  m_Vertices.push_back(p2);
}

void PrimitiveGeometry::PushTri(const Vector3f& p0, const Vector3f& p1, const Vector3f& p2) {
  Vector3f normal((p2-p1).cross(p0-p1).normalized());
  PushTri(Vertex(p0, normal), Vertex(p1, normal), Vertex(p2, normal));
}

void PrimitiveGeometry::PushQuad(const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, const Vector3f& p3) {
  // The orientation is given by the counterclockwise traversal of the points using the right-hand rule.

  static const Vector2f TEX_COORDS[4] = {
    Vector2f(0.0f, 0.0f),
    Vector2f(1.0f, 0.0f),
    Vector2f(1.0f, 1.0f),
    Vector2f(0.0f, 1.0f)
  };

  // Compute the normal for each triangle, where the quad is split up into 2 triangles, having its diagonal between points p0 and p2.
  Vector3f n0((p2-p1).cross(p0-p1).normalized());
  Vector3f n1((p3-p2).cross(p0-p2).normalized());
  
  PushTri(Vertex(p0, n0, TEX_COORDS[0]), Vertex(p1, n0, TEX_COORDS[1]), Vertex(p2, n0, TEX_COORDS[2]));
  PushTri(Vertex(p0, n1, TEX_COORDS[0]), Vertex(p2, n1, TEX_COORDS[2]), Vertex(p3, n1, TEX_COORDS[3]));
}
