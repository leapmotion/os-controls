#pragma once

#include "IContext.h"

namespace OculusRift {

class Context : public Hmd::IContext {
public:
  virtual ~Context ();

  virtual void Initialize () override;
  virtual bool IsInitialized () override;
  virtual void Shutdown () override;

  // TODO: enumeration of devices?

private:
  Context();
  friend class ::Hmd::HmdFactory;

  bool m_is_initialized;
};

} // end of namespace OculusRift
