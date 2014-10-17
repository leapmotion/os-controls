#pragma once

#include "Leap/Hmd/IEyeConfiguration.h"
#include "OVR.h"

namespace Leap {
namespace OculusRift {

class EyeConfiguration : public Hmd::IEyeConfiguration {
public:

  EyeConfiguration (ovrEyeType eye_type, const ovrFovPort &fov_port);
  virtual ~EyeConfiguration () { }

  virtual Hmd::EyeName EyeName () const override;
  virtual Hmd::DoubleArray<4*4> ProjectionMatrix (double near_clip, double far_clip, Hmd::MatrixComponentOrder matrix_component_order) const override;

private:

  ovrEyeType m_eye_type;
  ovrFovPort m_fov_port;
};

} // end of namespace OculusRift
} // end of namespace Leap
