#pragma once

namespace Hmd {

//A forward declaration useful to implementors
class HmdFactory;

class IContext {
public:

  virtual ~IContext () { }

  virtual void Initialize () = 0;
  virtual bool IsInitialized () = 0;
  virtual void Shutdown () = 0;

  // TODO: enumeration of devices?
};

} // end of namespace Hmd
