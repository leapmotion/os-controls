#pragma once

#include "Leap/GL/Camera.h"

namespace Leap {
namespace GL {

// Implements the Camera interface to provide a symmetric orthographic projection
// (symmetric in the sense that the view box is symmetric in each of x and y).
class OrthographicCamera : public Camera
{
public:

  OrthographicCamera ();
  virtual ~OrthographicCamera () { }

  // Define the camera in terms of the width/height/near depth/far depth of the orthographic view box.
  void SetViewBox (double width, double height, double near_clip_depth, double far_clip_depth);

  virtual const EigenTypes::Matrix4x4 &ProjectionMatrix () const override;

  double Width () const { return m_width; }
  double Height () const { return m_height; }
  double NearClipDepth () const { return m_near_clip_depth; }
  double FarClipDepth () const { return m_far_clip_depth; }

private:

  double m_width;
  double m_height;
  double m_near_clip_depth;
  double m_far_clip_depth;

  mutable bool m_projection_matrix_is_cached;
  mutable EigenTypes::Matrix4x4 m_projection_matrix;
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
