#pragma once

#include "EigenTypes.h"
#include "Leap/GL/GLBuffer.h"
#include "Leap/GL/GLVertexBuffer.h"
#include "Leap/GL/Mesh.h"
#include "RenderState.h"

#include <map>
#include <vector>

namespace Leap {
namespace GL {

class GLShader;

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.

typedef Mesh<GLVertexAttribute<GL_FLOAT_VEC3>, // Position
             GLVertexAttribute<GL_FLOAT_VEC3>, // Normal vector
             GLVertexAttribute<GL_FLOAT_VEC2>, // 2D texture coordinate
             GLVertexAttribute<GL_FLOAT_VEC4>  // RGBA color
            > PrimitiveGeometryMesh;

namespace PrimitiveGeometry {

// Factory functions for generating some simple shapes.  These functions assume that the draw mode (see Draw) is GL_TRIANGLES.
void PushUnitSphere(size_t resolution, PrimitiveGeometryMesh &mesh);
void PushUnitCylinder(size_t radialResolution, size_t verticalResolution, PrimitiveGeometryMesh &mesh);
void PushUnitSquare(PrimitiveGeometryMesh &mesh);
void PushUnitDisk(size_t resolution, PrimitiveGeometryMesh &mesh);
void PushUnitBox(PrimitiveGeometryMesh &mesh);

} // end of namespace PrimitiveGeometry
