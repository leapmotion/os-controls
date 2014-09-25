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

  enum class ClearOption { KEEP_INTERMEDIATE_DATA, CLEAR_INTERMEDIATE_DATA };
  
  PrimitiveGeometry();
 
  // structure for eliminating duplicate vertices in a map
  class Vertex {
  public:
    
    Vertex(const EigenTypes::Vector3f& pos, const EigenTypes::Vector3f& normal, const EigenTypes::Vector2f& texCoord = EigenTypes::Vector2f::Zero(), const Color &color = Color::White()) {
      p[0] =      pos[0]; p[1] =      pos[1];    p[2] = pos[2];
      n[0] =   normal[0]; n[1] =   normal[1]; n[2] = normal[2];
      t[0] = texCoord[0]; t[1] = texCoord[1];
      c[0] =   color.R(); c[1] =   color.G(); c[2] = color.B(); c[3] = color.A();
    }
    // This defines a dictionary ordering on the array elements stored within and can be used to remove duplicates.
    bool operator<(const Vertex& other) const {
      return memcmp(reinterpret_cast<const void*>(this), reinterpret_cast<const void*>(&other), sizeof(Vertex)) < 0;
    }

    // These are used in glVertexAttribPointer to determine the offset of the various attributes into the buffer data.
    static size_t OffsetOfPositionAttribute () { return offsetof(Vertex, p); }
    static size_t OffsetOfNormalAttribute ()   { return offsetof(Vertex, n); }
    static size_t OffsetOfTexCoordAttribute () { return offsetof(Vertex, t); }
    static size_t OffsetOfColorAttribute ()    { return offsetof(Vertex, c); }
  
  private:
    
    float p[3];
    float n[3];
    float t[2];
    float c[4];
  };

  // Gives direct access to the list of vertices.
  std::vector<Vertex> &Vertices () { return m_Vertices; }
  
  // functions for manipulating GPU-side buffers
  void CleanUpBuffers();
  void UploadDataToBuffers(ClearOption clear_option = ClearOption::CLEAR_INTERMEDIATE_DATA);

  // after geometry is uploaded, draws the geometry using the current render state
  void Draw(const GLShader &bound_shader, GLenum drawMode) const;

  // Factory functions for generating some simple shapes.  These functions assume that the draw mode (see Draw) is GL_TRIANGLES.
  static PrimitiveGeometry CreateUnitSphere(int resolution);
  static PrimitiveGeometry CreateUnitCylinder(int radialResolution, int verticalResolution);
  static PrimitiveGeometry CreateUnitSquare();
  static PrimitiveGeometry CreateUnitDisk(int resolution);
  static PrimitiveGeometry CreateUnitBox();

  // Add a quad with the specified points, winding counterclockwise, with the given normal EigenTypes::Vector, adding
  // texture coordinates if requested.  This function assumes that the draw mode (see Draw) is GL_TRIANGLES.
  void PushTri (const Vertex &p0, const Vertex &p1, const Vertex &p2);
  // Add a quad with the specified points, winding counterclockwise, with the given normal EigenTypes::Vector, adding
  // texture coordinates if requested.  This function assumes that the draw mode (see Draw) is GL_TRIANGLES.
  void PushTri (const EigenTypes::Vector3f &p0, const EigenTypes::Vector3f &p1, const EigenTypes::Vector3f &p2);
  // Add a quad with the specified points, winding counterclockwise, with the given normal EigenTypes::Vector, adding
  // texture coordinates if requested.  This function assumes that the draw mode (see Draw) is GL_TRIANGLES.
  void PushQuad (const EigenTypes::Vector3f &p0, const EigenTypes::Vector3f &p1, const EigenTypes::Vector3f &p2, const EigenTypes::Vector3f &p3);
  
private:

  typedef std::map<Vertex, unsigned int, std::less<Vertex>> VertexIndexMap;
  
  // This intermediate storage for vertex data as it is being generated, and may be cleared during upload.
  std::vector<Vertex> m_Vertices;
  // This is the VBO that stores the indexed vertex data (no duplicate vertices).
  GLBuffer m_VertexBuffer;
  // This is the number of indices used to pass to glDrawElements when drawing the VBO.
  int m_NumIndices;
  // This is the buffer containing the index elements.
  GLBuffer m_IndexBuffer;
};
