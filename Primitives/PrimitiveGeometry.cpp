#include "PrimitiveGeometry.h"

PrimitiveGeometry::PrimitiveGeometry() : m_NumIndices(0) { }

void PrimitiveGeometry::CleanUpBuffers() {
  if (m_VertexBuffer.IsCreated()) {
    m_VertexBuffer.Destroy();
  }
  if (m_NormalBuffer.IsCreated()) {
    m_NormalBuffer.Destroy();
  }
  if (m_IndexBuffer.IsCreated()) {
    m_IndexBuffer.Destroy();
  }
  m_Vertices.clear();
  m_Normals.clear();
}

void PrimitiveGeometry::UploadDataToBuffers() {
  VertexIndexMap vboMap;
  std::vector<float> vertexData;
  std::vector<float> normalData;
  std::vector<unsigned int> indexData;
  
  // eliminate duplicate vertices using a temporary map
  assert(m_Vertices.size() == m_Normals.size());
  for (size_t i=0; i<m_Vertices.size(); i++) {
    MapVertex cur(m_Vertices[i], m_Normals[i]);
    const VertexIndexMap::iterator result = vboMap.find(cur);
    if (result == vboMap.end()) {
      unsigned int newIndex = static_cast<unsigned int>(vertexData.size()/3);
      vertexData.push_back(cur.p[0]);
      vertexData.push_back(cur.p[1]);
      vertexData.push_back(cur.p[2]);
      normalData.push_back(cur.n[0]);
      normalData.push_back(cur.n[1]);
      normalData.push_back(cur.n[2]);
      indexData.push_back(newIndex);
      vboMap[cur] = newIndex;
    } else {
      indexData.push_back(result->second);
    }
  }

  m_VertexBuffer.Create(GL_ARRAY_BUFFER);
  m_VertexBuffer.Bind();
  m_VertexBuffer.Allocate(static_cast<void*>(vertexData.data()), static_cast<int>(vertexData.size()*sizeof(float)), GL_STATIC_DRAW);
  m_VertexBuffer.Release();

  m_NormalBuffer.Create(GL_ARRAY_BUFFER);
  m_NormalBuffer.Bind();
  m_NormalBuffer.Allocate(static_cast<void*>(normalData.data()), static_cast<int>(normalData.size()*sizeof(float)), GL_STATIC_DRAW);
  m_NormalBuffer.Release();

  m_IndexBuffer.Create(GL_ELEMENT_ARRAY_BUFFER);
  m_IndexBuffer.Bind();
  m_IndexBuffer.Allocate(static_cast<void*>(indexData.data()), static_cast<int>(indexData.size()*sizeof(unsigned int)), GL_STATIC_DRAW);
  m_IndexBuffer.Release();

  m_NumIndices = static_cast<int>(indexData.size());
}

void PrimitiveGeometry::Draw(RenderState& renderState, GLenum drawMode) {
  const bool haveVertices = m_VertexBuffer.IsCreated();
  const bool haveNormals = m_NormalBuffer.IsCreated();
  const bool haveColors = m_ColorBuffer.IsCreated();
  const bool haveTexCoords = m_TexCoordBuffer.IsCreated();

  if (haveVertices) {
    m_VertexBuffer.Bind();
    renderState.EnablePositionAttribute();
  }

  if (haveNormals) {
    m_NormalBuffer.Bind();
    renderState.EnableNormalAttribute();
  }

  if (haveColors) {
    m_ColorBuffer.Bind();
    renderState.EnableColorAttribute();
  }

  if (haveTexCoords) {
    m_TexCoordBuffer.Bind();
    renderState.EnableTexCoordAttribute();
  }

  m_IndexBuffer.Bind();
  glDrawElements(drawMode, m_NumIndices, GL_UNSIGNED_INT, 0);
  m_IndexBuffer.Release();

  if (haveVertices) {
    renderState.DisablePositionAttribute();
    m_VertexBuffer.Release();
  }

  if (haveNormals) {
    renderState.DisableNormalAttribute();
    m_NormalBuffer.Release();
  }

  if (haveColors) {
    renderState.DisableColorAttribute();
    m_ColorBuffer.Release();
  }

  if (haveTexCoords) {
    renderState.DisableTexCoordAttribute();
    m_TexCoordBuffer.Release();
  }
}

PrimitiveGeometry PrimitiveGeometry::CreateUnitSphere(int resolution) {
  PrimitiveGeometry geom;
  stdvectorV3f& vertices = geom.m_Vertices;
  stdvectorV3f& normals = geom.m_Normals;

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

      // add vertices
      vertices.push_back(v1);
      vertices.push_back(v2);
      vertices.push_back(v3);
      vertices.push_back(v4);
      vertices.push_back(v5);
      vertices.push_back(v6);

      // add normals (for a sphere, just normalize vertex position)
      normals.push_back(v1.normalized());
      normals.push_back(v2.normalized());
      normals.push_back(v3.normalized());
      normals.push_back(v4.normalized());
      normals.push_back(v5.normalized());
      normals.push_back(v6.normalized());
    }
  }

  geom.UploadDataToBuffers();
  return geom;
}

PrimitiveGeometry PrimitiveGeometry::CreateUnitCylinder(int radialResolution, int verticalResolution) {
  PrimitiveGeometry geom;
  stdvectorV3f& vertices = geom.m_Vertices;
  stdvectorV3f& normals = geom.m_Normals;

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

      vertices.push_back(v1);
      vertices.push_back(v2);
      vertices.push_back(v3);

      vertices.push_back(v4);
      vertices.push_back(v3);
      vertices.push_back(v2);

      normals.push_back(n1);
      normals.push_back(n1);
      normals.push_back(n2);

      normals.push_back(n2);
      normals.push_back(n2);
      normals.push_back(n1);
    }
  }

  geom.UploadDataToBuffers();
  return geom;
}

PrimitiveGeometry PrimitiveGeometry::CreateUnitSquare() {
  PrimitiveGeometry geom;
  stdvectorV3f& vertices = geom.m_Vertices;
  stdvectorV3f& normals = geom.m_Normals;

  const Vector3f normal(Vector3f::UnitZ());
  const Vector3f p1(-1.0f, -1.0f, 0.0f);
  const Vector3f p2(1.0f, -1.0f, 0.0f);
  const Vector3f p3(1.0f, 1.0f, 0.0f);
  const Vector3f p4(-1.0f, 1.0f, 0.0f);

  vertices.push_back(p1);
  vertices.push_back(p2);
  vertices.push_back(p3);
  vertices.push_back(p3);
  vertices.push_back(p4);
  vertices.push_back(p1);
  
  // all vertices have the same normal
  for (size_t i=0; i<vertices.size(); i++) {
    normals.push_back(normal);
  }

  geom.UploadDataToBuffers();
  return geom;
}

PrimitiveGeometry PrimitiveGeometry::CreateUnitDisk(int resolution) {
  PrimitiveGeometry geom;
  stdvectorV3f& vertices = geom.m_Vertices;
  stdvectorV3f& normals = geom.m_Normals;

  const Vector3f normal(Vector3f::UnitZ());
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

    vertices.push_back(p1);
    vertices.push_back(p2);
    vertices.push_back(center);

    normals.push_back(normal);
    normals.push_back(normal);
    normals.push_back(normal);
  }

  geom.UploadDataToBuffers();
  return geom;
}


PrimitiveGeometry PrimitiveGeometry::CreateUnitBox() {
  PrimitiveGeometry geom;
  stdvectorV3f& vertices = geom.m_Vertices;
  stdvectorV3f& normals = geom.m_Normals;

  const Vector3f p1(-1.0f, -1.0f, -1.0f);
  const Vector3f p2(1.0f, -1.0f, -1.0f);
  const Vector3f p3(1.0f, 1.0f, -1.0f);
  const Vector3f p4(-1.0f, 1.0f, -1.0f);
  const Vector3f p5(-1.0f, -1.0f, 1.0f);
  const Vector3f p6(1.0f, -1.0f, 1.0f);
  const Vector3f p7(1.0f, 1.0f, 1.0f);
  const Vector3f p8(-1.0f, 1.0f, 1.0f);

  vertices.push_back(p5);
  vertices.push_back(p6);
  vertices.push_back(p8);

  vertices.push_back(p6);
  vertices.push_back(p8);
  vertices.push_back(p7);


  vertices.push_back(p6);
  vertices.push_back(p2);
  vertices.push_back(p7);

  vertices.push_back(p3);
  vertices.push_back(p2);
  vertices.push_back(p7);


  vertices.push_back(p2);
  vertices.push_back(p1);
  vertices.push_back(p3);

  vertices.push_back(p4);
  vertices.push_back(p1);
  vertices.push_back(p3);


  vertices.push_back(p1);
  vertices.push_back(p5);
  vertices.push_back(p4);

  vertices.push_back(p8);
  vertices.push_back(p5);
  vertices.push_back(p4);


  vertices.push_back(p1);
  vertices.push_back(p2);
  vertices.push_back(p5);

  vertices.push_back(p6);
  vertices.push_back(p2);
  vertices.push_back(p5);


  vertices.push_back(p8);
  vertices.push_back(p7);
  vertices.push_back(p4);

  vertices.push_back(p3);
  vertices.push_back(p7);
  vertices.push_back(p4);

  for (size_t i=0; i<vertices.size(); i++) {
    normals.push_back(vertices[i].normalized());
  }

  geom.UploadDataToBuffers();
  return geom;
}
