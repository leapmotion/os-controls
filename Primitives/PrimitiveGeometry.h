#pragma once

#include "Color.h"
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

  // structure for eliminating duplicate vertices in a map
  struct Vertex {
    Vertex(const Vector3f& pos, const Vector3f& normal, const Vector2f& texCoord = Vector2f::Zero(), const Color &color = Color::White()) {
      p[0] =      pos[0]; p[1] =      pos[1];    p[2] = pos[2];
      n[0] =   normal[0]; n[1] =   normal[1]; n[2] = normal[2];
      t[0] = texCoord[0]; t[1] = texCoord[1];
      c[0] =   color.R(); c[1] =   color.G(); c[2] = color.B(); c[3] = color.A();
    }
    float p[3];
    float n[3];
    float t[2];
    float c[4];
    bool operator<(const Vertex& other) const {
      return memcmp(reinterpret_cast<const void*>(this), reinterpret_cast<const void*>(&other), sizeof(Vertex)) < 0;
    }
  };

  // functions for manually manipulating geometry
//   stdvectorV3f& Vertices() { return m_Vertices; }
//   stdvectorV3f& Normals() { return m_Normals; }
  std::vector<Vertex> &VertexAttributes () { return m_VertexAttributes; }

  // functions for manipulating GPU-side buffers
  void CleanUpBuffers();
  void UploadDataToBuffers();

  // after geometry is uploaded, draws the geometry using the current render state
  void Draw(const GLShader &bound_shader, GLenum drawMode) const;

  // factory functions for generating some simple shapes
  static PrimitiveGeometry CreateUnitSphere(int resolution);
  static PrimitiveGeometry CreateUnitCylinder(int radialResolution, int verticalResolution);
  static PrimitiveGeometry CreateUnitSquare(); // this actually creates a square that is 2x2, not a unit square (which is typically an AAB with corners (0,0) and (1,1)).
  static PrimitiveGeometry CreateUnitDisk(int resolution);
  static PrimitiveGeometry CreateUnitBox();

  // Add a quad with the specified points, winding counterclockwise, with the given normal vector, adding texture coordinates if requested.
  void PushTri (const Vertex &p0, const Vertex &p1, const Vertex &p2);
  // Add a quad with the specified points, winding counterclockwise, with the given normal vector, adding texture coordinates if requested.
  void PushTri (const Vector3f &p0, const Vector3f &p1, const Vector3f &p2);
  // Add a quad with the specified points, winding counterclockwise, with the given normal vector, adding texture coordinates if requested.
  void PushQuad (const Vector3f &p0, const Vector3f &p1, const Vector3f &p2, const Vector3f &p3);
  
private:

  typedef std::map<Vertex, unsigned int, std::less<Vertex>> VertexIndexMap;
  
  std::vector<Vertex> m_VertexAttributes;
//   stdvectorV3f m_Vertices;
//   stdvectorV3f m_Normals;
//   stdvectorV2f m_TexCoords;
//   GLBuffer m_VertexBuffer;
//   GLBuffer m_NormalBuffer;
  GLBuffer m_VertexAttributeBuffer;
  int m_NumIndices;
  GLBuffer m_IndexBuffer;
//   GLBuffer m_ColorBuffer;
//   GLBuffer m_TexCoordBuffer;
};
