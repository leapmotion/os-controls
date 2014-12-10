#include <cassert>
#include <cmath>
#include "Leap/GL/PerspectiveCamera.h"
#include <limits>

namespace Leap {
namespace GL {

PerspectiveCamera::PerspectiveCamera ()
  : m_projection_matrix_is_cached(false)
{
  // Reasonable defaults: 90 degree FOV, aspect ratio of 1, near clip depth = 1, far clip depth = 1000.
  SetUsingFOVAndAspectRatio(3.14159265358 / 2.0, 1.0, 1.0, 1000.0);
}

void PerspectiveCamera::SetUsingFOVAndAspectRatio (double horiz_FOV_radians, double width_over_height, double near_clip_depth, double far_clip_depth) {
  assert(horiz_FOV_radians >= 0.0);
  assert(width_over_height > std::numeric_limits<double>::epsilon());
  assert(near_clip_depth > std::numeric_limits<double>::epsilon());
  assert(far_clip_depth - near_clip_depth > std::numeric_limits<double>::epsilon());

  m_projection_matrix_is_cached = false;

  m_horiz_FOV_radians = horiz_FOV_radians;
  m_width_over_height = width_over_height;
  m_near_clip_depth = near_clip_depth;
  m_far_clip_depth = far_clip_depth;

  // Derive the other values.
  m_near_clip_width = 2.0 * near_clip_depth * std::tan(m_horiz_FOV_radians / 2.0);
  m_near_clip_height = m_near_clip_width / m_width_over_height;

  Camera::SetPerspectiveProjectionMatrix_UsingFOVAndAspectRatio(m_projection_matrix, horiz_FOV_radians, width_over_height, near_clip_depth, far_clip_depth);
  m_projection_matrix_is_cached = true;
}

void PerspectiveCamera::SetUsingFrustumNearClipSize (double near_clip_width, double near_clip_height, double near_clip_depth, double far_clip_depth)
{
  assert(near_clip_width > std::numeric_limits<double>::epsilon());
  assert(near_clip_height > std::numeric_limits<double>::epsilon());
  assert(near_clip_depth > std::numeric_limits<double>::epsilon());
  assert(far_clip_depth - near_clip_depth > std::numeric_limits<double>::epsilon());

  m_projection_matrix_is_cached = false;

  m_near_clip_width = near_clip_width;
  m_near_clip_height = near_clip_height;
  m_near_clip_depth = near_clip_depth;
  m_far_clip_depth = far_clip_depth;

  // Derive the other values.
  m_width_over_height = m_near_clip_width / m_near_clip_height;
  m_horiz_FOV_radians = 2.0 * std::atan(m_near_clip_width / (2.0 * m_near_clip_depth));

  Camera::SetPerspectiveProjectionMatrix_UsingSymmetricFrustumNearClipSize(m_projection_matrix, near_clip_width, near_clip_height, near_clip_depth, far_clip_depth);
  m_projection_matrix_is_cached = true;
}

const EigenTypes::Matrix4x4 &PerspectiveCamera::ProjectionMatrix () const {
  if (!m_projection_matrix_is_cached) {
    const double right = m_near_clip_width / 2.0;
    const double left = -right;
    const double top = m_near_clip_height / 2.0;
    const double bottom = -top;
    Camera::SetPerspectiveProjectionMatrix(m_projection_matrix, left, right, bottom, top, m_near_clip_depth, m_far_clip_depth);
    m_projection_matrix_is_cached = true;
  }
  return m_projection_matrix;
}

} // end of namespace GL
} // end of namespace Leap
