#include <cassert>
#include "Leap/GL/OrthographicCamera.h"
#include <limits>

namespace Leap {
namespace GL {

OrthographicCamera::OrthographicCamera ()
  : m_projection_matrix_is_cached(false)
{
  // Reasonable defaults: width = 2, height = 2, near clip depth = 1, far clip depth = 1000.
  SetSymmetricViewBox(2.0, 2.0, 1.0, 1000.0);
}

void OrthographicCamera::SetSymmetricViewBox (double width, double height, double near_clip_depth, double far_clip_depth) {
  assert(std::abs(width) > std::numeric_limits<double>::epsilon());
  assert(std::abs(height) > std::numeric_limits<double>::epsilon());
  assert(std::abs(far_clip_depth - near_clip_depth) > std::numeric_limits<double>::epsilon());

  m_projection_matrix_is_cached = false;

  m_width = width;
  m_height = height;
  m_right = m_width / 2.0;
  m_left = -m_right;
  m_top = m_height / 2.0;
  m_bottom = -m_top;
  m_near_clip_depth = near_clip_depth;
  m_far_clip_depth = far_clip_depth;
}

void OrthographicCamera::SetViewBox (double left, double right, double bottom, double top, double near_clip_depth, double far_clip_depth) {
  assert(std::abs(right - left) > std::numeric_limits<double>::epsilon());
  assert(std::abs(top - bottom) > std::numeric_limits<double>::epsilon());
  assert(std::abs(far_clip_depth - near_clip_depth) > std::numeric_limits<double>::epsilon());

  m_projection_matrix_is_cached = false;

  m_left = left;
  m_right = right;
  m_bottom = bottom;
  m_top = top;
  m_width = m_right - m_left;
  m_height = m_top - m_bottom;
  m_near_clip_depth = near_clip_depth;
  m_far_clip_depth = far_clip_depth;
}

const EigenTypes::Matrix4x4 &OrthographicCamera::ProjectionMatrix () const {
  if (!m_projection_matrix_is_cached) {
    Camera::SetOrthographicProjectionMatrix(m_projection_matrix, m_left, m_right, m_bottom, m_top, m_near_clip_depth, m_far_clip_depth);
    m_projection_matrix_is_cached = true;
  }
  return m_projection_matrix;
}

} // end of namespace GL
} // end of namespace Leap
