#pragma once

#include "IException.h"
#include "OculusRift/RiftContext.h"
#include "OculusRift/RiftDevice.h"

namespace Leap {
namespace OculusRift {

class Exception : public Hmd::IException {
public:

  Exception (const std::string &message, const OculusRift::Context *context = nullptr, const OculusRift::Device *device = nullptr)
    :
    m_message(message),
    m_context(context),
    m_device(device)
  { }
  virtual ~Exception () { }

  virtual const char *what () const override { return m_message.c_str(); }
  // Returns the OculusRift::Context to which this Exception applies, or nullptr if not specified/defined.
  virtual const OculusRift::Context *Context () const override { return m_context; }
  // Returns the OculusRift::Device to which this Exception applies, or nullptr if not specified/defined.
  virtual const OculusRift::Device *Device () const override { return m_device; }

private:

  std::string m_message;
  const OculusRift::Context *m_context;
  const OculusRift::Device *m_device;
};


} // end of namespace OculusRift
} // end of namespace Leap
