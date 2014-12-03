#pragma once

#include "Leap/GL/Camera.h"

namespace Leap {
namespace GL {

// Implements the Camera interface to provide a symmetric perspective projection
// (symmetric in the sense that the view frustum is symmetric in each of x and y).
class PerspectiveCamera : public Camera
{
public:

  PerspectiveCamera ();
  virtual ~PerspectiveCamera () { }

  // Define the camera in terms of its horizontal FOV, its view aspect ratio (width over height), and the near/far depth of the view frustum.
  void SetUsingFOVAndAspectRatio (double horiz_FOV_radians, double width_over_height, double near_clip_depth, double far_clip_depth);
  // Define the camera in terms of the width/height/depth of the near side of the view frustum and the depth of the far side of the view frustum.
  void SetUsingFrustumNearClipSize (double near_clip_width, double near_clip_height, double near_clip_depth, double far_clip_depth);

  virtual const EigenTypes::Matrix4x4 &ProjectionMatrix () const override;

  double HorizFOVRadians () const { return m_horiz_FOV_radians; }
  double WidthOverHeight () const { return m_width_over_height; }
  double NearClipWidth () const { return m_near_clip_width; }
  double NearClipHeight () const { return m_near_clip_height; }
  double NearClipDepth () const { return m_near_clip_depth; }
  double FarClipDepth () const { return m_far_clip_depth; }

private:

  double m_horiz_FOV_radians;
  double m_width_over_height;
  double m_near_clip_width;
  double m_near_clip_height;
  double m_near_clip_depth;
  double m_far_clip_depth;

  mutable bool m_projection_matrix_is_cached;
  mutable EigenTypes::Matrix4x4 m_projection_matrix;
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
