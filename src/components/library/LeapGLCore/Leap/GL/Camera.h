#pragma once

#include "EigenTypes.h"

namespace Leap {
namespace GL {

// Camera provides two transformations:
// - A projection matrix (as in the fixed function pipeline of older versions of OpenGL)
// - A view matrix (as in the "view" part of the model-view matrix in OpenGL)
// There are two implementations of this interface:
// - OrthographicCamera : Provides the projection matrix corresponding to a symmetric orthographic view box.
// - PerspectiveCamera  : Provides the projection matrix corresponding to a symmetric view frustum.
// Asymmetric versions of each of these could be made, but aren't yet called for.  The use case for
// asymmetric projections is when the center of the viewport is not aligned with the "lookat" direction.
// This could be used, for example, in a tile renderer, where the viewport is divided up into "tiles"
// and each tile is rendered separately.
class Camera {
public:

  virtual ~Camera () { }

  virtual const EigenTypes::Matrix4x4 &ProjectionMatrix () const = 0;

  // Compute the transformation taking points in world coordinates to points in view coordinates,
  // where the positive x axis goes rightward in the view, the positive y axis goes upward in the
  // view, and the positive z axis goes toward the viewer (away from focus_position).  The
  // transformation is defined by an eye position (position of the viewer), a focus position (a
  // point that the viewer is looking at), and an "up" direction (the viewer's notion of "up").
  // This transformation is inverse to the one computed by ComputeViewToWorldTransformation.
  // This is mathematically equivalent to gluLookAt.
  static void ComputeWorldToViewTransformation (
    EigenTypes::Matrix3x3 &linear,             // Out-variable for the linear transformation part of the affine transformation
    EigenTypes::Vector3 &translation,          // Out-variable for the translation part of the affine transformation.
    const EigenTypes::Vector3 &eye_position,   // In-variable for the location of the eye.
    const EigenTypes::Vector3 &focus_position, // In-variable for the point the eye is looking at.
    const EigenTypes::Vector3 &up_direction);  // In-variable for the "up" direction for the view.
  // Compute the transformation taking points in view coordinates to points in world coordinates.
  // This transformation is inverse to the one computed by ComputeWorldToViewTransformation.
  // This transformation is what you would use as the local AffineTransformation property in order
  // to place a physical viewer as a SceneGraphNode in a scene graph.
  static void ComputeViewToWorldTransformation (
    EigenTypes::Matrix3x3 &linear,             // Out-variable for the linear transformation part of the affine transformation
    EigenTypes::Vector3 &translation,          // Out-variable for the translation part of the affine transformation.
    const EigenTypes::Vector3 &eye_position,   // In-variable for the location of the eye.
    const EigenTypes::Vector3 &focus_position, // In-variable for the point the eye is looking at.
    const EigenTypes::Vector3 &up_direction);  // In-variable for the "up" direction for the view.
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
