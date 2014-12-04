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
}

const EigenTypes::Matrix4x4 &PerspectiveCamera::ProjectionMatrix () const {
  if (!m_projection_matrix_is_cached) {
    const double denom = 1.0 / (m_near_clip_depth - m_far_clip_depth);
    const double l00 = 2.0 * m_near_clip_depth / m_near_clip_width;
    const double l11 = 2.0 * m_near_clip_depth / m_near_clip_height;
    const double l22 = (m_near_clip_depth + m_far_clip_depth) * denom;
    const double t2 = 2.0 * m_near_clip_depth * m_far_clip_depth*denom;
    m_projection_matrix << l00, 0.0,  0.0, 0.0,
                           0.0, l11,  0.0, 0.0,
                           0.0, 0.0,  l22,  t2,
                           0.0, 0.0, -1.0, 0.0;
    m_projection_matrix_is_cached = true;
  }
  return m_projection_matrix;
}

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
