#include <cassert>
#include <cmath>
#include "Leap/GL/Camera.h"
#include <limits>

namespace Leap {
namespace GL {

void ComputeViewFrameInWorldCoordinates (
  EigenTypes::Vector3 &x,
  EigenTypes::Vector3 &y,
  EigenTypes::Vector3 &z,
  const EigenTypes::Vector3 &eye_position,
  const EigenTypes::Vector3 &focus_position,
  const EigenTypes::Vector3 &up_direction)
{
  assert(std::abs(up_direction.squaredNorm() - 1.0) < std::numeric_limits<double>::epsilon() && "up_direction must be a unit-length vector.");
  y = up_direction;

  z = eye_position - focus_position;
  assert(z.squaredNorm() > std::numeric_limits<double>::epsilon());
  z.normalize();

  x = y.cross(z);
  assert(x.squaredNorm() > std::numeric_limits<double>::epsilon());
  x.normalize();

  y = z.cross(x); // z and x should be relatively orthonormal, so no normalization of y should be necessary.
  assert(std::abs(y.squaredNorm() - 1.0) < std::numeric_limits<double>::epsilon());
}

void Camera::ComputeWorldToViewTransformation (
  EigenTypes::Matrix3x3 &linear,
  EigenTypes::Vector3 &translation,
  const EigenTypes::Vector3 &eye_position,
  const EigenTypes::Vector3 &focus_position,
  const EigenTypes::Vector3 &up_direction)
{
  EigenTypes::Vector3 x, y, z;
  ComputeViewFrameInWorldCoordinates(x, y, z, eye_position, focus_position, up_direction);

  linear.row(0) = x.transpose();
  linear.row(1) = y.transpose();
  linear.row(2) = z.transpose();
  translation = linear * -eye_position;
}

static void ComputeViewToWorldTransformation (
  EigenTypes::Matrix3x3 &linear,
  EigenTypes::Vector3 &translation,
  const EigenTypes::Vector3 &eye_position,
  const EigenTypes::Vector3 &focus_position,
  const EigenTypes::Vector3 &up_direction)
{
  EigenTypes::Vector3 x, y, z;
  ComputeViewFrameInWorldCoordinates(x, y, z, eye_position, focus_position, up_direction);

  linear.col(0) = x;
  linear.col(1) = y;
  linear.col(2) = z;
  translation = eye_position;
}

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
