#pragma once

#include "GLBuffer.h"
#include "EigenTypes.h"
#include "RenderState.h"
#include <vector>
#include <map>

class PrimitiveGeometry {

public:

  PrimitiveGeometry();

  // functions for manually manipulating geometry
  stdvectorV3f& Vertices() { return m_Vertices; }
  stdvectorV3f& Normals() { return m_Normals; }

  // functions for manipulating GPU-side buffers
  void CleanUpBuffers();
  void UploadDataToBuffers();

  // after geometry is uploaded, draws the geometry using the current render state
  void Draw(RenderState& renderState, GLenum drawMode);

  // factory functions for generating some simple shapes
  static PrimitiveGeometry CreateUnitSphere(int resolution);
  static PrimitiveGeometry CreateUnitCylinder(int radialResolution, int verticalResolution);
  static PrimitiveGeometry CreateUnitSquare();
  static PrimitiveGeometry CreateUnitDisk(int resolution);
  static PrimitiveGeometry CreateUnitBox();

private:

  // structure for eliminating duplicate vertices in a map
  struct MapVertex {
    MapVertex(const Vector3f& pos, const Vector3f& normal) {
      p[0] = pos[0];
      p[1] = pos[1];
      p[2] = pos[2];
      n[0] = normal[0];
      n[1] = normal[1];
      n[2] = normal[2];
    }
    float p[3];
    float n[3];
    bool operator<(const MapVertex& other) const {
      return memcmp((const void*)this, (const void*)(&other), sizeof(MapVertex)) > 0;
    }
  };

  typedef std::map<MapVertex, unsigned int, std::less<MapVertex>, Eigen::aligned_allocator<std::pair<MapVertex, unsigned int> > > VertexIndexMap;

  int m_NumIndices;
  stdvectorV3f m_Vertices;
  stdvectorV3f m_Normals;
  GLBuffer m_VertexBuffer;
  GLBuffer m_NormalBuffer;
  GLBuffer m_IndexBuffer;
  GLBuffer m_ColorBuffer;
  GLBuffer m_TexCoordBuffer;

};
