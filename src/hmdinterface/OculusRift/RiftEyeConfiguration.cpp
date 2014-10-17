#include "RiftException.h"
#include "RiftEyeConfiguration.h"

#include <cassert>

namespace OculusRift {

EyeConfiguration::EyeConfiguration (ovrEyeType eye_type, const ovrFovPort &fov_port)
  : m_eye_type(eye_type)
  , m_fov_port(fov_port)
{
  if (m_eye_type < 0 || m_eye_type >= ovrEye_Count)
    throw OculusRift::Exception("Invalid eye type.");
}

Hmd::EyeName EyeConfiguration::EyeName () const {
  switch (m_eye_type) {
    case ovrEye_Left: return Hmd::EyeName::LEFT;
    case ovrEye_Right: return Hmd::EyeName::RIGHT;
    default: throw OculusRift::Exception("Invalid eye type.");
  }
}

Hmd::DoubleArray<4*4> EyeConfiguration::ProjectionMatrix (double near_clip, double far_clip, Hmd::MatrixComponentOrder matrix_component_order) const {
  // OVR is a little weird in that:
  // 1. It has a pretty good templatized C++ Matrix4 class but also a crappy C structure with identical layout.
  // 2. Going in between the C++ class and the C structure is not made very easy.
  OVR::Matrix4<float> projection_matrix_f(ovrMatrix4f_Projection(m_fov_port, float(near_clip), float(far_clip), true));
  // OVR stores matrices in row-major order, so only tranpose the matrix if column-major has been requested.
  if (matrix_component_order == Hmd::MatrixComponentOrder::COLUMN_MAJOR) {
    projection_matrix_f.Transpose();
  }
  // Convert the float matrix that we got from OVR to a double matrix.
  OVR::Matrix4<double> projection_matrix_d(projection_matrix_f);
  // This will memcpy the contents from projection_matrix_d.
  return Hmd::DoubleArray<4*4>(projection_matrix_d);
}

} // end of namespace OculusRift
