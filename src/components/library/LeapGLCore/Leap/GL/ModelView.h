#pragma once

#include "EigenTypes.h"

namespace Leap {
namespace GL {

class ModelView {
public:
  ModelView();
  const EigenTypes::Matrix4x4& Matrix() const;
  EigenTypes::Matrix4x4& Matrix();
  void Reset();
  void LookAt(const EigenTypes::Vector3& eye, const EigenTypes::Vector3& center, const EigenTypes::Vector3& up);
  void Translate(const EigenTypes::Vector3& translation);
  void Rotate(const EigenTypes::Vector3& axis, double angleRadians);
  void Scale(const EigenTypes::Vector3& scale);
  void Multiply(const EigenTypes::Matrix4x4& transform);
  // This takes a Matrix3x3 M and uses it to Multiply by the block 4x4 matrix:
  //   [ M 0 ]
  //   [ 0 1 ]
  void Multiply(const EigenTypes::Matrix3x3& transform);
  void Push();
  void Pop();
  void Clear();
private:

  // Compute the transformation taking points in world coordinates to points in view coordinates,
  // where the positive x axis goes rightward in the view, the positive y axis goes upward in the
  // view, and the positive z axis goes toward the viewer (away from focus_position).  The
  // transformation is defined by an eye position (position of the viewer), a focus position (a
  // point that the viewer is looking at), and an "up" direction (the viewer's notion of "up").
  // This transformation is inverse to the one computed by LookAt_Inverse.
  // Equivalent to the deprecated function gluLookAt.
  static void LookAt (
    EigenTypes::Matrix4x4 &model_view_matrix,  // Out-variable for the transformation.
    const EigenTypes::Vector3 &eye_position,   // In-variable for the location of the eye.
    const EigenTypes::Vector3 &focus_position, // In-variable for the point the eye is looking at.
    const EigenTypes::Vector3 &up_direction);  // In-variable for the "up" direction for the view.

  // Compute the transformation taking points in view coordinates to points in world coordinates.
  // This transformation is inverse to the one computed by LookAt.
  // This transformation is what you would use as the local AffineTransformation property in order
  // to place a physical viewer as a SceneGraphNode in a scene graph.  The out-variable is not
  // called `model_view_matrix` because the value computed here would not be used in a model_view
  // matrix for viewing purposes -- it is for physical placement of the viewer in the world, rather
  // than placement of the world about the viewer.
  static void LookAt_Inverse (
    EigenTypes::Matrix4x4 &matrix,             // Out-variable for the transformation.
    const EigenTypes::Vector3 &eye_position,   // In-variable for the location of the eye.
    const EigenTypes::Vector3 &focus_position, // In-variable for the point the eye is looking at.
    const EigenTypes::Vector3 &up_direction);  // In-variable for the "up" direction for the view.

  std::vector<EigenTypes::Matrix4x4, Eigen::aligned_allocator<EigenTypes::Matrix4x4> > m_stack;
};

} // end of namespace GL
} // end of namespace Leap
