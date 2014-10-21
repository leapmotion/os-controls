#pragma once

#include "IEyeConfiguration.h"
#include "OVR.h"

namespace OculusRift {

class EyeConfiguration : public Hmd::IEyeConfiguration {
public:

  EyeConfiguration (ovrEyeType eye_type, const ovrFovPort &fov_port, const ovrEyeRenderDesc &eye_render_desc);
  virtual ~EyeConfiguration () { }

  virtual Hmd::EyeName EyeName () const override;
  virtual Hmd::IntermediateArray<double,4*4> ProjectionMatrix (double near_clip, double far_clip, Hmd::MatrixComponentOrder matrix_component_order) const override;

  // non-interface, Oculus-specific methods -- NOTE: these are subject to change, so don't use unless you know what you're doing!
  const ovrEyeRenderDesc &EyeRenderDesc () const { return m_eye_render_desc; }

private:

  ovrEyeType m_eye_type;
  ovrFovPort m_fov_port;
  ovrEyeRenderDesc m_eye_render_desc;
};

} // end of namespace OculusRift
