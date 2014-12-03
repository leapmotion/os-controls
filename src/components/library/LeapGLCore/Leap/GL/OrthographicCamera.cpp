#include <cassert>
#include "Leap/GL/OrthographicCamera.h"
#include <limits>

namespace Leap {
namespace GL {

OrthographicCamera::OrthographicCamera ()
  : m_projection_matrix_is_cached(false)
{
  // Reasonable defaults: width = 2, height = 2, near clip depth = 1, far clip depth = 1000.
  SetViewBox(2.0, 2.0, 1.0, 1000.0);
}

void OrthographicCamera::SetViewBox (double width, double height, double near_clip_depth, double far_clip_depth)
{
  assert(width > std::numeric_limits<double>::epsilon());
  assert(height > std::numeric_limits<double>::epsilon());
  assert(near_clip_depth > std::numeric_limits<double>::epsilon());
  assert(far_clip_depth - near_clip_depth > std::numeric_limits<double>::epsilon());

  m_width = width;
  m_height = height;
  m_near_clip_depth = near_clip_depth;
  m_far_clip_depth = far_clip_depth;
}

const EigenTypes::Matrix4x4 &OrthographicCamera::ProjectionMatrix () const {
  if (!m_projection_matrix_is_cached) {
    const double denom = 1.0 / (m_far_clip_depth - m_near_clip_depth);
    const double d0 = 2.0 / m_width;
    const double d1 = 2.0 / m_height;
    const double d2 = 2.0 * denom;
    const double t2 = (m_near_clip_depth + m_far_clip_depth) * denom;
    m_projection_matrix <<  d0, 0.0, 0.0, 0.0,
                           0.0,  d1, 0.0, 0.0,
                           0.0, 0.0,  d2,  t2,
                           0.0, 0.0, 0.0, 1.0;
    m_projection_matrix_is_cached = true;
  }
  return m_projection_matrix;
}

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
