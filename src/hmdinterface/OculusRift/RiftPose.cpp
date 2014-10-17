#include "RiftPose.h"

#include <cmath>

namespace OculusRift {

/// @brief Convenience function for squaring an expression without evaluating it twice.
template <typename T_>
T_ sqr (const T_ &t) {
  return t*t;
}

Hmd::DoubleArray<3> Pose::Position () const {
  return Hmd::DoubleArray<3>(m_pose.Translation);
}

Hmd::DoubleArray<4> Pose::OrientationQuaternion (Hmd::QuaternionNormalization quaternion_normalization) const {
  OVR::Quat<double> q(m_pose.Rotation);
  if (quaternion_normalization == Hmd::QuaternionNormalization::REQUIRED) {
    q /= q.Length();
  }
  // This dumb step is necessary because OVR has chosen the convention of storing
  // a quaternion w+ix+jy+kz as (x,y,z,w), instead of (w,x,y,z).
  Hmd::DoubleArray<4> retval;
  retval[0] = q.w;
  retval[1] = q.x;
  retval[2] = q.y;
  retval[3] = q.z;
  return retval;
}

Hmd::DoubleArray<3*3> Pose::OrientationMatrix (Hmd::MatrixComponentOrder matrix_component_order) const {
  OVR::Matrix3<double> orientation_matrix(m_pose.Rotation);
  // OVR stores their matrices in row-major order, so we only need to do something
  // if MatrixComponentOrder::COLUMN_MAJOR has been requested.
  if (matrix_component_order == Hmd::MatrixComponentOrder::COLUMN_MAJOR) {
    orientation_matrix.Transpose();
  }
  return Hmd::DoubleArray<3*3>(orientation_matrix);
}

Hmd::DoubleArray<4*4> Pose::TotalMatrix (Hmd::MatrixComponentOrder matrix_component_order) const {
  OVR::Matrix4<double> total_matrix(m_pose);
  // OVR stores their matrices in row-major order, so we only need to do something
  // if MatrixComponentOrder::COLUMN_MAJOR has been requested.
  if (matrix_component_order == Hmd::MatrixComponentOrder::COLUMN_MAJOR) {
    total_matrix.Transpose();
  }
  return Hmd::DoubleArray<4*4>(total_matrix);
}

} // end of namespace OculusRift
