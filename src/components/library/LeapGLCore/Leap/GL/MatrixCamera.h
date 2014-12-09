#pragma once

#include "Leap/GL/Camera.h"

namespace Leap {
namespace GL {

// Implements the Camera interface to provide the most generally specifiable
// projection matrix -- by specifying the projection matrix directly.
class MatrixCamera : public Camera
{
public:

  MatrixCamera () { m_projection_matrix.setIdentity(); }
  virtual ~MatrixCamera () { }

  void SetProjectionMatrix (const EigenTypes::Matrix4x4 &projection_matrix) { m_projection_matrix = projection_matrix; }
  // This method can be used to access the projection matrix to do in-place manipulations on it.  This would normally
  // be named ProjectionMatrix, but this name has been chosen to explicitly distinguish it from the Camera virtual
  // method ProjectionMatrix.
  EigenTypes::Matrix4x4 &GetProjectionMatrix () { return m_projection_matrix; }

  virtual const EigenTypes::Matrix4x4 &ProjectionMatrix () const override { return m_projection_matrix; }

private:

  mutable EigenTypes::Matrix4x4 m_projection_matrix;
};

} // end of namespace GL
} // end of namespace Leap
