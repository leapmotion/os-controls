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
//
// Notes on Orthographic projection
//
// - SetOrthographic provides a means to compute the projection matrix for
//   an arbitrary, axially-aligned orthographic view box (not necessarily symmetric
//   about the x and y axes).
// - SetOrthographic_UsingSymmetricViewBox provides a means to compute the
//   projection matrix for a symmetric axially-aligned orthographic view box (symmetric in
//   the sense that the view box is symmetric in each of x and y).
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
//
// Notes on Perspective projection
//
// TODO
namespace Projection {

// Compute the transformation taking points in world coordinates to points in view coordinates,
// where the positive x axis goes rightward in the view, the positive y axis goes upward in the
// view, and the positive z axis goes toward the viewer (away from focus_position).  The
// transformation is defined by an eye position (position of the viewer), a focus position (a
// point that the viewer is looking at), and an "up" direction (the viewer's notion of "up").
// This transformation is inverse to the one computed by ComputeViewToWorldTransformation.
// Equivalent to the deprecated function gluLookAt.
// TODO: this should go in ModelView
void ComputeWorldToViewTransformation (
  EigenTypes::Matrix3x3 &linear,             // Out-variable for the linear transformation part of the affine transformation.
  EigenTypes::Vector3 &translation,          // Out-variable for the translation part of the affine transformation.
  const EigenTypes::Vector3 &eye_position,   // In-variable for the location of the eye.
  const EigenTypes::Vector3 &focus_position, // In-variable for the point the eye is looking at.
  const EigenTypes::Vector3 &up_direction);  // In-variable for the "up" direction for the view.
// Compute the transformation taking points in view coordinates to points in world coordinates.
// This transformation is inverse to the one computed by ComputeWorldToViewTransformation.
// This transformation is what you would use as the local AffineTransformation property in order
// to place a physical viewer as a SceneGraphNode in a scene graph.
// TODO: this should go in ModelView
void ComputeViewToWorldTransformation (
  EigenTypes::Matrix3x3 &linear,             // Out-variable for the linear transformation part of the affine transformation.
  EigenTypes::Vector3 &translation,          // Out-variable for the translation part of the affine transformation.
  const EigenTypes::Vector3 &eye_position,   // In-variable for the location of the eye.
  const EigenTypes::Vector3 &focus_position, // In-variable for the point the eye is looking at.
  const EigenTypes::Vector3 &up_direction);  // In-variable for the "up" direction for the view.

// Equivalent to the deprecated OpenGL function glOrtho (see OpenGL 2.1 API docs).
void SetOrthographic (
  EigenTypes::Matrix4x4 &projection_matrix,
  double left, double right,
  double bottom, double top,
  double near_clip_depth, double far_clip_depth);
// Define the camera in terms of the width/height/near depth/far depth of a symmetric orthographic view box.
void SetOrthographic_UsingSymmetricViewBox (
  EigenTypes::Matrix4x4 &projection_matrix,
  double width, double height,
  double near_clip_depth, double far_clip_depth);

// Equivalent to the deprecated OpenGL function glFrustum (see OpenGL 2.1 API docs).
void SetPerspective (
  EigenTypes::Matrix4x4 &projection_matrix,
  double near_clip_left, double near_clip_right,
  double near_clip_bottom, double near_clip_top,
  double near_clip_depth, double far_clip_depth);
// Define the camera in terms of its horizontal FOV, its view aspect ratio (width over height),
// and the near/far depth of the view frustum.  See PerspectiveCamera.
void SetPerspective_UsingFOVAndAspectRatio (
  EigenTypes::Matrix4x4 &projection_matrix,
  double horiz_FOV_radians, double width_over_height,
  double near_clip_depth, double far_clip_depth);
// Define the camera in terms of the width/height/depth of the near side of the view frustum
// and the depth of the far side of the view frustum.  See PerspectiveCamera.
void SetPerspective_UsingSymmetricFrustumNearClipSize (
  EigenTypes::Matrix4x4 &projection_matrix,
  double near_clip_width, double near_clip_height,
  double near_clip_depth, double far_clip_depth);

} // end of namespace Projection

} // end of namespace GL
} // end of namespace Leap
