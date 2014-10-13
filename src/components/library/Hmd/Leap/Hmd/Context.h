#pragma once

namespace Leap {
namespace Hmd {

class Context {
public:

  virtual ~Context () { }

  virtual void Initialize () = 0;
  virtual bool IsInitialized () = 0;
  virtual void Shutdown () = 0;

  // TODO: enumeration of devices?
};

} // end of namespace Hmd
} // end of namespace Leap
