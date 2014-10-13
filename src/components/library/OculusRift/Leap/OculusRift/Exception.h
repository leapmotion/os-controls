#pragma once

#include "Leap/Hmd/Exception.h"

namespace Leap {
namespace OculusRift {

class Context;
class Device;

class Exception : public Hmd::Exception {
public:

  Exception (const std::string &message, const OculusRift::Context *context = nullptr, const OculusRift::Device *device = nullptr)
    :
    m_message(message),
    m_context(context),
    m_device(device)
  { }
  virtual ~Exception () { }

  virtual const char *what () const noexcept override { return m_message.c_str(); }
  // Returns the Hmd::Context to which this Exception applies, or nullptr if not specified/defined.
  virtual const OculusRift::Context *Context () const override { return m_context; }
  // Returns the Hmd::Device to which this Exception applies, or nullptr if not specified/defined.
  virtual const OculusRift::Device *Device () const override { return m_device; }

private:

  std::string m_message;
  const OculusRift::Context *m_context;
  const OculusRift::Device *m_device;
};


} // end of namespace OculusRift
} // end of namespace Leap
