#pragma once

#include "IEyeConfiguration.h"
#include "IPose.h"
#include "OVR.h"

namespace OculusRift {

class Pose : public Hmd::IPose {
public:

  Pose (const OVR::Pose<float> &pose) : m_pose(pose) { }
  Pose (const OVR::Pose<double> &pose) : m_pose(pose) { }
  virtual ~Pose () { }

  virtual Hmd::DoubleArray<3> Position () const override;
  virtual Hmd::DoubleArray<4> OrientationQuaternion (Hmd::QuaternionNormalization quaternion_normalization) const override;
  virtual Hmd::DoubleArray<3*3> OrientationMatrix (Hmd::MatrixComponentOrder matrix_component_order) const override;
  virtual Hmd::DoubleArray<4*4> TotalMatrix (Hmd::MatrixComponentOrder matrix_component_order) const override;
  virtual Hmd::DoubleArray<4*4> ViewMatrix (Hmd::MatrixComponentOrder matrix_component_order, const Hmd::IEyeConfiguration &eye_configuration) const override;

private:

  OVR::Pose<double> m_pose;
};

} // end of namespace OculusRift
