#include "PrimitiveGeometry.h"

#include <iostream> // TEMP

#include "GLShader.h"

PrimitiveGeometry::PrimitiveGeometry() : m_NumIndices(0) { }

void PrimitiveGeometry::CleanUpBuffers() {
  m_VertexAttributes.clear();
  if (m_VertexAttributeBuffer.IsCreated()) {
    m_VertexAttributeBuffer.Destroy();
  }
//   if (m_VertexBuffer.IsCreated()) {
//     m_VertexBuffer.Destroy();
//   }
//   if (m_NormalBuffer.IsCreated()) {
//     m_NormalBuffer.Destroy();
//   }
  m_NumIndices = 0;
  if (m_IndexBuffer.IsCreated()) {
    m_IndexBuffer.Destroy();
  }
//   if (m_TexCoordBuffer.IsCreated()) {
//     m_TexCoordBuffer.Destroy();
//   }
//   m_Vertices.clear();
//   m_Normals.clear();
//   m_TexCoords.clear();
}

void PrimitiveGeometry::UploadDataToBuffers() {
  VertexIndexMap vertex_index_map;
//   std::vector<float> vertexData;
//   std::vector<float> normalData;
//   std::vector<float> texCoordData;
  std::vector<unsigned int> indexData;

//   const bool haveTexCoords = !m_TexCoords.empty();
  
  // eliminate duplicate vertices using a temporary map
//   assert(m_Vertices.size() == m_Normals.size());
//   for (size_t i = 0; i < m_VertexAttributes.size(); ++i) {
  for (auto it = m_VertexAttributes.begin(); it != m_VertexAttributes.end(); ++it) {
    const Vertex &vertex = *it;
    auto mapped_vertex = vertex_index_map.find(vertex);
    // If the current vertex is not in the vbo map already, add it.
    if (mapped_vertex == vertex_index_map.end()) {
      unsigned int newIndex = vertex_index_map.size();
      indexData.push_back(newIndex);
      vertex_index_map[vertex] = newIndex;
    } else { // Otherwise, add the existing vertex's index.
      indexData.push_back(mapped_vertex->second);
    }
  }
  
/*
    
    Vector2f texCoord = Vector2f::Zero();
    if (haveTexCoords) {
      texCoord = m_TexCoords[i];
    }
    MapVertex cur(m_Vertices[i], m_Normals[i], texCoord);
    const VertexIndexMap::iterator result = vertex_index_map.find(cur);
    if (result == vertex_index_map.end()) {
      unsigned int newIndex = static_cast<unsigned int>(vertexData.size()/3);
      vertexData.push_back(cur.p[0]);
      vertexData.push_back(cur.p[1]);
      vertexData.push_back(cur.p[2]);
      normalData.push_back(cur.n[0]);
      normalData.push_back(cur.n[1]);
      normalData.push_back(cur.n[2]);
      if (haveTexCoords) {
        texCoordData.push_back(cur.t[0]);
        texCoordData.push_back(cur.t[1]);
      }
      indexData.push_back(newIndex);
      vertex_index_map[cur] = newIndex;
    } else {
      indexData.push_back(result->second);
    }
  }*/

  m_VertexAttributeBuffer.Create(GL_ARRAY_BUFFER);
  m_VertexAttributeBuffer.Bind();
  m_VertexAttributeBuffer.Allocate(static_cast<void*>(m_VertexAttributes.data()), static_cast<int>(m_VertexAttributes.size()*sizeof(Vertex)), GL_STATIC_DRAW);
  m_VertexAttributeBuffer.Unbind();

//   m_VertexBuffer.Create(GL_ARRAY_BUFFER);
//   m_VertexBuffer.Bind();
//   m_VertexBuffer.Allocate(static_cast<void*>(vertexData.data()), static_cast<int>(vertexData.size()*sizeof(float)), GL_STATIC_DRAW);
//   m_VertexBuffer.Unbind();
// 
//   m_NormalBuffer.Create(GL_ARRAY_BUFFER);
//   m_NormalBuffer.Bind();
//   m_NormalBuffer.Allocate(static_cast<void*>(normalData.data()), static_cast<int>(normalData.size()*sizeof(float)), GL_STATIC_DRAW);
//   m_NormalBuffer.Unbind();

  m_NumIndices = static_cast<int>(indexData.size());

  m_IndexBuffer.Create(GL_ELEMENT_ARRAY_BUFFER);
  m_IndexBuffer.Bind();
  m_IndexBuffer.Allocate(static_cast<void*>(indexData.data()), static_cast<int>(indexData.size()*sizeof(unsigned int)), GL_STATIC_DRAW);
  m_IndexBuffer.Unbind();

//   if (haveTexCoords) {
//     m_TexCoordBuffer.Create(GL_ARRAY_BUFFER);
//     m_TexCoordBuffer.Bind();
//     m_TexCoordBuffer.Allocate(static_cast<void*>(texCoordData.data()), static_cast<int>(texCoordData.size()*sizeof(float)), GL_STATIC_DRAW);
//     m_TexCoordBuffer.Unbind();
//   }

}

void PrimitiveGeometry::Draw(const GLShader &bound_shader, GLenum drawMode) const {
//   const bool haveVertices = m_VertexBuffer.IsCreated() && bound_shader.HasAttribute("position");
//   const bool haveNormals = m_NormalBuffer.IsCreated() && bound_shader.HasAttribute("normal");
//   const bool haveColors = m_ColorBuffer.IsCreated() && bound_shader.HasAttribute("color");
//   const bool haveTexCoords = m_TexCoordBuffer.IsCreated() && bound_shader.HasAttribute("tex_coord");
// 
//   if (haveVertices) {
//     GLint loc = bound_shader.LocationOfAttribute("position");
//     glEnableVertexAttribArray(loc);
//     m_VertexBuffer.Bind();
//     glVertexAttribPointer(loc, 3, GL_FLOAT, GL_TRUE, 0, 0);
//     m_VertexBuffer.Unbind();
//   }
// 
//   if (haveNormals) {
//     GLint loc = bound_shader.LocationOfAttribute("normal");
//     glEnableVertexAttribArray(loc);
//     m_NormalBuffer.Bind();
//     glVertexAttribPointer(loc, 3, GL_FLOAT, GL_TRUE, 0, 0);
//     m_NormalBuffer.Unbind();
//   }
// 
//   if (haveColors) {
//     GLint loc = bound_shader.LocationOfAttribute("color");
//     glEnableVertexAttribArray(loc);
//     m_ColorBuffer.Bind();
//     glVertexAttribPointer(loc, 3, GL_FLOAT, GL_TRUE, 0, 0);
//     m_ColorBuffer.Unbind();
//   }
// 
//   if (haveTexCoords) {
//     GLint loc = bound_shader.LocationOfAttribute("tex_coord");
//     glEnableVertexAttribArray(loc);
//     m_TexCoordBuffer.Bind();
//     glVertexAttribPointer(loc, 2, GL_FLOAT, GL_TRUE, 0, 0);
//     m_TexCoordBuffer.Unbind();
//   }

  GLint position_loc = bound_shader.LocationOfAttribute("position");
  GLint normal_loc = bound_shader.LocationOfAttribute("normal");
  GLint tex_coord_loc = bound_shader.LocationOfAttribute("tex_coord");
//   GLint color_loc = bound_shader.LocationOfAttribute("color");
  
  glEnableVertexAttribArray(position_loc);
  glEnableVertexAttribArray(normal_loc);
  glEnableVertexAttribArray(tex_coord_loc);
//   glEnableVertexAttribArray(color_loc);
  
  m_VertexAttributeBuffer.Bind();
  glVertexAttribPointer(position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, p)));
  glVertexAttribPointer(normal_loc, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, n)));
  glVertexAttribPointer(tex_coord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, t)));
//   glVertexAttribPointer(color_loc, 4, GL_FLOAT, GL_FALSE, sizeof(MapVertex), reinterpret_cast<void*>(offsetof(Vertex, c)));
  m_VertexAttributeBuffer.Unbind();
  
  m_IndexBuffer.Bind();
  glDrawElements(drawMode, m_NumIndices, GL_UNSIGNED_INT, 0);
  m_IndexBuffer.Unbind();

  glDisableVertexAttribArray(position_loc);
  glDisableVertexAttribArray(normal_loc);
  glDisableVertexAttribArray(tex_coord_loc);
//   glDisableVertexAttribArray(color_loc);
  
//   if (haveVertices) {
// //     m_VertexBuffer.Bind();
//     glDisableVertexAttribArray(bound_shader.LocationOfAttribute("position"));
// //     m_VertexBuffer.Unbind();
//   }
// 
//   if (haveNormals) {
// //     m_NormalBuffer.Bind();
//     glDisableVertexAttribArray(bound_shader.LocationOfAttribute("normal"));
// //     m_NormalBuffer.Unbind();
//   }
// 
//   if (haveColors) {
// //     m_ColorBuffer.Bind();
//     glDisableVertexAttribArray(bound_shader.LocationOfAttribute("color"));
// //     m_ColorBuffer.Unbind();
//   }
// 
//   if (haveTexCoords) {
// //     m_TexCoordBuffer.Bind();
//     glDisableVertexAttribArray(bound_shader.LocationOfAttribute("tex_coord"));
// //     m_TexCoordBuffer.Unbind();
//   }
}

PrimitiveGeometry PrimitiveGeometry::CreateUnitSphere(int resolution) {
  PrimitiveGeometry geom;
//   stdvectorV3f& vertices = geom.m_Vertices;
//   stdvectorV3f& normals = geom.m_Normals;

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
//       // add vertices
//       vertices.push_back(v1);
//       vertices.push_back(v2);
//       vertices.push_back(v3);
//       vertices.push_back(v4);
//       vertices.push_back(v5);
//       vertices.push_back(v6);
// 
//       // add normals (for a sphere, just normalize vertex position)
//       normals.push_back(v1.normalized());
//       normals.push_back(v2.normalized());
//       normals.push_back(v3.normalized());
//       normals.push_back(v4.normalized());
//       normals.push_back(v5.normalized());
//       normals.push_back(v6.normalized());
    }
  }

  geom.UploadDataToBuffers();
  return geom;
}

PrimitiveGeometry PrimitiveGeometry::CreateUnitCylinder(int radialResolution, int verticalResolution) {
  PrimitiveGeometry geom;
//   stdvectorV3f& vertices = geom.m_Vertices;
//   stdvectorV3f& normals = geom.m_Normals;

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
//       vertices.push_back(v1);
//       vertices.push_back(v2);
//       vertices.push_back(v3);
// 
//       vertices.push_back(v4);
//       vertices.push_back(v3);
//       vertices.push_back(v2);
// 
//       normals.push_back(n1);
//       normals.push_back(n1);
//       normals.push_back(n2);
// 
//       normals.push_back(n2);
//       normals.push_back(n2);
//       normals.push_back(n1);
    }
  }

  geom.UploadDataToBuffers();
  return geom;
}

PrimitiveGeometry PrimitiveGeometry::CreateUnitSquare() {
  PrimitiveGeometry geom;
  
  geom.PushQuad(Vector3f(-1.0f, -1.0f, 0.0f), Vector3f(1.0f, -1.0f, 0.0f), Vector3f(1.0f, 1.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
  
//   stdvectorV3f& vertices = geom.m_Vertices;
//   stdvectorV3f& normals = geom.m_Normals;
//   stdvectorV2f& texCoords = geom.m_TexCoords;
// 
//   const Vector3f normal(Vector3f::UnitZ());
//   const Vector3f p1(-1.0f, -1.0f, 0.0f);
//   const Vector3f p2(1.0f, -1.0f, 0.0f);
//   const Vector3f p3(1.0f, 1.0f, 0.0f);
//   const Vector3f p4(-1.0f, 1.0f, 0.0f);
// 
//   vertices.push_back(p1);
//   vertices.push_back(p2);
//   vertices.push_back(p3);
//   vertices.push_back(p3);
//   vertices.push_back(p4);
//   vertices.push_back(p1);
// 
//   texCoords.push_back(p1.head<2>()*0.5f + Vector2f::Constant(0.5f));
//   texCoords.push_back(p2.head<2>()*0.5f + Vector2f::Constant(0.5f));
//   texCoords.push_back(p3.head<2>()*0.5f + Vector2f::Constant(0.5f));
//   texCoords.push_back(p3.head<2>()*0.5f + Vector2f::Constant(0.5f));
//   texCoords.push_back(p4.head<2>()*0.5f + Vector2f::Constant(0.5f));
//   texCoords.push_back(p1.head<2>()*0.5f + Vector2f::Constant(0.5f));
//   
//   // all vertices have the same normal
//   for (size_t i=0; i<vertices.size(); i++) {
//     normals.push_back(normal);
//   }

  geom.UploadDataToBuffers();
  return geom;
}

PrimitiveGeometry PrimitiveGeometry::CreateUnitDisk(int resolution) {
  PrimitiveGeometry geom;
//   stdvectorV3f& vertices = geom.m_Vertices;
//   stdvectorV3f& normals = geom.m_Normals;

//   const Vector3f normal(Vector3f::UnitZ());
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
//     vertices.push_back(p1);
//     vertices.push_back(p2);
//     vertices.push_back(center);
// 
//     normals.push_back(normal);
//     normals.push_back(normal);
//     normals.push_back(normal);
  }

  geom.UploadDataToBuffers();
  return geom;
}


PrimitiveGeometry PrimitiveGeometry::CreateUnitBox() {
  PrimitiveGeometry geom;

  const Vector3f p000(-1.0f, -1.0f, -1.0f);
  const Vector3f p001(-1.0f, -1.0f,  1.0f);
  const Vector3f p010(-1.0f,  1.0f, -1.0f);
  const Vector3f p011(-1.0f,  1.0f,  1.0f);
  const Vector3f p100( 1.0f, -1.0f, -1.0f);
  const Vector3f p101( 1.0f, -1.0f,  1.0f);
  const Vector3f p110( 1.0f,  1.0f, -1.0f);
  const Vector3f p111( 1.0f,  1.0f,  1.0f);

  geom.PushQuad(p010, p000, p001, p011);
  geom.PushQuad(p100, p110, p111, p101);
  geom.PushQuad(p000, p100, p101, p001);
  geom.PushQuad(p110, p010, p011, p111);
  geom.PushQuad(p010, p110, p100, p000);
  geom.PushQuad(p001, p101, p111, p011);
  
//   stdvectorV3f& vertices = geom.m_Vertices;
//   stdvectorV3f& normals = geom.m_Normals;
// 
//   const Vector3f p1(-1.0f, -1.0f, -1.0f);
//   const Vector3f p2( 1.0f, -1.0f, -1.0f);
//   const Vector3f p3( 1.0f,  1.0f, -1.0f);
//   const Vector3f p4(-1.0f,  1.0f, -1.0f);
//   const Vector3f p5(-1.0f, -1.0f,  1.0f);
//   const Vector3f p6( 1.0f, -1.0f,  1.0f);
//   const Vector3f p7( 1.0f,  1.0f,  1.0f);
//   const Vector3f p8(-1.0f,  1.0f,  1.0f);
// 
//   vertices.push_back(p5);
//   vertices.push_back(p6);
//   vertices.push_back(p8);
// 
//   vertices.push_back(p6);
//   vertices.push_back(p8);
//   vertices.push_back(p7);
// 
// 
//   vertices.push_back(p6);
//   vertices.push_back(p2);
//   vertices.push_back(p7);
// 
//   vertices.push_back(p3);
//   vertices.push_back(p2);
//   vertices.push_back(p7);
// 
// 
//   vertices.push_back(p2);
//   vertices.push_back(p1);
//   vertices.push_back(p3);
// 
//   vertices.push_back(p4);
//   vertices.push_back(p1);
//   vertices.push_back(p3);
// 
// 
//   vertices.push_back(p1);
//   vertices.push_back(p5);
//   vertices.push_back(p4);
// 
//   vertices.push_back(p8);
//   vertices.push_back(p5);
//   vertices.push_back(p4);
// 
// 
//   vertices.push_back(p1);
//   vertices.push_back(p2);
//   vertices.push_back(p5);
// 
//   vertices.push_back(p6);
//   vertices.push_back(p2);
//   vertices.push_back(p5);
// 
// 
//   vertices.push_back(p8);
//   vertices.push_back(p7);
//   vertices.push_back(p4);
// 
//   vertices.push_back(p3);
//   vertices.push_back(p7);
//   vertices.push_back(p4);
// 
//   for (size_t i=0; i<vertices.size(); i++) {
//     normals.push_back(vertices[i].normalized());
//   }
//   for (size_t i=0; i<geom.m_Vertices.size(); i++) {
//     geom.m_Normals.push_back(geom.m_Vertices[i].normalized());
//   }

  geom.UploadDataToBuffers();
  return geom;
}

void PrimitiveGeometry::PushTri(const Vertex& p0, const Vertex& p1, const Vertex& p2) {
  // The orientation is given by the counterclockwise traversal of the points using the right-hand rule.
  
//   size_t vertex_base_index = m_Vertices.size();
  
//   m_Vertices.push_back(p0);
//   m_Vertices.push_back(p1);
//   m_Vertices.push_back(p2);
  
//   Vector3f center(p0 + p1 + p2);
//   center /= 3.0f;
  
//   Vector3f n((p2-p1).cross(p0-p1));
//   n.normalize();
  
  m_VertexAttributes.push_back(p0);
  m_VertexAttributes.push_back(p1);
  m_VertexAttributes.push_back(p2);
  
//   for (size_t i = 0; i < 3; ++i) {
//     m_Normals.push_back((n + 0.1f*(m_Vertices[vertex_base_index+i]-center).normalized()).normalized());
//   }
//   for (size_t i = 0; i < 3; ++i) {
//     m_Normals.push_back(normal);
//   }
//   for (size_t i = 0; i < 3; ++i) {
//     m_Normals.push_back(m_Vertices[vertex_base_index+i].normalized());
//   }
}

void PrimitiveGeometry::PushTri(const Vector3f& p0, const Vector3f& p1, const Vector3f& p2) {
  Vector3f n((p2-p1).cross(p0-p1).normalized());
  PushTri(Vertex(p0, n), Vertex(p1, n), Vertex(p2, n));
}

void PrimitiveGeometry::PushQuad(const Vector3f& p0, const Vector3f& p1, const Vector3f& p2, const Vector3f& p3) {
  // The orientation is given by the counterclockwise traversal of the points using the right-hand rule.

  static const Vector2f TEX_COORDS[4] = {
    Vector2f(0.0f, 0.0f),
    Vector2f(1.0f, 0.0f),
    Vector2f(1.0f, 1.0f),
    Vector2f(0.0f, 1.0f)
  };

  Vector3f n0((p2-p1).cross(p0-p1).normalized());
  PushTri(Vertex(p0, n0, TEX_COORDS[0]), Vertex(p1, n0, TEX_COORDS[1]), Vertex(p2, n0, TEX_COORDS[2]));
  
  Vector3f n1((p3-p2).cross(p0-p2).normalized());
  PushTri(Vertex(p0, n1, TEX_COORDS[0]), Vertex(p2, n1, TEX_COORDS[2]), Vertex(p3, n1, TEX_COORDS[3]));
  
//   size_t vertex_base_index = m_Vertices.size();
  
//   m_Vertices.push_back(p0);
//   m_Vertices.push_back(p1);
//   m_Vertices.push_back(p2);
//   
//   m_Vertices.push_back(p0);
//   m_Vertices.push_back(p2);
//   m_Vertices.push_back(p3);

//   for (size_t i = 0; i < 6; ++i) {
//     m_Normals.push_back(normal);
//   }
//   for (size_t i = 0; i < 6; ++i) {
//     m_Normals.push_back(m_Vertices[vertex_base_index+i].normalized());
//   }

//   if (add_tex_coords) {
//     // These are constructed to correspond exactly to the points p0, p1, p2, p3 in that order.
//     static const Vector2f TEX_COORDS[4] = {
//       Vector2f(0.0f, 0.0f),
//       Vector2f(1.0f, 0.0f),
//       Vector2f(1.0f, 1.0f),
//       Vector2f(0.0f, 1.0f)
//     };
//     
//     m_TexCoords.push_back(TEX_COORDS[0]);
//     m_TexCoords.push_back(TEX_COORDS[1]);
//     m_TexCoords.push_back(TEX_COORDS[2]);
//     
//     m_TexCoords.push_back(TEX_COORDS[0]);
//     m_TexCoords.push_back(TEX_COORDS[2]);
//     m_TexCoords.push_back(TEX_COORDS[3]);
//   }
}
