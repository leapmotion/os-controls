#pragma once

#include "Leap/GL/Camera.h"

namespace Leap {
namespace GL {

// Implements the Camera interface to provide either a symmetric orthographic projection
// (symmetric in the sense that the view box is symmetric in each of x and y), or an
// asymmetric orthographic projection.
//
// Before understanding what the orthographic projection is, it's important to note
// that the "view" coordinates are a right-handed coordinate system that have the
// positive x axis extending to the viewer's right, the positive y axis extending
// upward along the view, and the positive z axis extending toward the viewer.
//
// The symmetric orthographic view box is the axially-aligned, oriented box
//   [-w/2,w/2] x [-h/2,h/2] x [-n,-f]
// where w := width, h := height, n := near clipping plane depth, and f := far clipping
// plane depth.  Note that f-n must be positive, and therefore -f < -n, so the oriented
// interval [-n,-f] is reversed on the number line.  The orthographic projection operation
// maps the oriented orthographic view box onto the oriented box
//   [-1,1] x [-1,1] x [-1,1]
// noting that -n in the 3rd coordinate maps to -1 and -f maps to 1.  The x and y coordinates
// in the projection will be used to determine the x and y coordinates on screen, while the
// z coordinate in the projection gives value that will be used in the depth buffer (lower
// depths are closer to the viewer).
//
// The asymmetric orthographic view box is similar to the symmetric case, except that it
// isn't necessarily symmetric in each of the x and y coordinates).  It's the axially-aligned,
// oriented box
//   [l,r] x [b,t] x [-n,-f]
// where l := left, r := right, b := bottom, t := top, and n and f are as before.
//
// Note that the projection operation is defined as follows.  The 3-vector U = (x,y,z) is
// "homogenized" to produce a 4-vector V = (x,y,z,1).  The projection matrix P is multiplied
// onto W (as a column vector) to produce a projected 4-vector W = P*V.  In order to get
// a 3-vector from the 4-vector W, it is re-projected into the hyperplane whose 4th coordinate
// is 1 -- this is done by dividing the whole vector through by its 4th coordinate.  For
// example,
//   (a,b,c,d) |-> (a/d, b/d, c/d, 1).
// The first three coordinates are taken as the projected 3-vector (in the example, the
// projected vector is (a/d, b/d, c/d)).  In the case of the orthographic projection, the
// 4th coordinate of the 4-vector V will always be 1, so the division step doesn't change
// anything.
//
// Some additional related documentation can be found at
// http://msdn.microsoft.com/en-us/library/windows/desktop/dd373965%28v=vs.85%29.aspx
class OrthographicCamera : public Camera
{
public:

  OrthographicCamera ();
  virtual ~OrthographicCamera () { }

  // Define the camera in terms of the width/height/near depth/far depth of a symmetric orthographic view box.
  void SetSymmetricViewBox (double width, double height, double near_clip_depth, double far_clip_depth);
  // Define the camera in terms of the left/right/bottom/top/near depth/far depth of a symmetric orthographic view box.
  // This does the same thing as the old OpenGL glOrtho.
  void SetViewBox (double left, double right, double bottom, double top, double near_clip_depth, double far_clip_depth);

  virtual const EigenTypes::Matrix4x4 &ProjectionMatrix () const override;

  double Left () const { return m_left; }
  double Right () const { return m_right; }
  double Bottom () const { return m_bottom; }
  double Top () const { return m_top; }
  double Width () const { return m_width; }
  double Height () const { return m_height; }
  double NearClipDepth () const { return m_near_clip_depth; }
  double FarClipDepth () const { return m_far_clip_depth; }

private:

  double m_left;
  double m_right;
  double m_bottom;
  double m_top;
  double m_width;
  double m_height;
  double m_near_clip_depth;
  double m_far_clip_depth;

  mutable bool m_projection_matrix_is_cached;
  mutable EigenTypes::Matrix4x4 m_projection_matrix;
};

} // end of namespace GL
} // end of namespace Leap
