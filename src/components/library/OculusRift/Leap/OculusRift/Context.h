#pragma once

#include "Leap/Hmd/Context.h"

namespace Leap {
namespace OculusRift {

class Context : public Leap::Hmd::Context {
public:

  Context ();
  virtual ~Context ();

  virtual void Initialize () override;
  virtual bool IsInitialized () override;
  virtual void Shutdown () override;

  // TODO: enumeration of devices?

private:

  bool m_is_initialized;
};

} // end of namespace OculusRift
} // end of namespace Leap
