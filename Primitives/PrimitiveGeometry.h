#pragma once

#include "EigenTypes.h"
#include "GLBuffer.h"
#include "RenderState.h"

#include <map>
#include <vector>

class GLShader;

// Convenience class for creating and managing GPU geometry.
// Simple shapes can be created/uploaded automatically using the available factory functions.
// More complex shapes can created by manually inputting triangles then calling UploadDataToBuffers()
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
  void Draw(const GLShader &bound_shader, GLenum drawMode);

  // factory functions for generating some simple shapes
  static PrimitiveGeometry CreateUnitSphere(int resolution);
  static PrimitiveGeometry CreateUnitCylinder(int radialResolution, int verticalResolution);
  static PrimitiveGeometry CreateUnitSquare();
  static PrimitiveGeometry CreateUnitDisk(int resolution);
  static PrimitiveGeometry CreateUnitBox();

private:

  // structure for eliminating duplicate vertices in a map
  struct MapVertex {
    MapVertex(const Vector3f& pos, const Vector3f& normal, const Vector2f& texCoord = Vector2f::Zero()) {
      p[0] = pos[0];
      p[1] = pos[1];
      p[2] = pos[2];
      n[0] = normal[0];
      n[1] = normal[1];
      n[2] = normal[2];
      t[0] = texCoord[0];
      t[1] = texCoord[1];
    }
    float p[3];
    float n[3];
    float t[2];
    bool operator<(const MapVertex& other) const {
      return memcmp((const void*)this, (const void*)(&other), sizeof(MapVertex)) > 0;
    }
  };

  typedef std::map<MapVertex, unsigned int, std::less<MapVertex>> VertexIndexMap;

  int m_NumIndices;
  stdvectorV3f m_Vertices;
  stdvectorV3f m_Normals;
  stdvectorV2f m_TexCoords;
  GLBuffer m_VertexBuffer;
  GLBuffer m_NormalBuffer;
  GLBuffer m_IndexBuffer;
  GLBuffer m_ColorBuffer;
  GLBuffer m_TexCoordBuffer;
};
