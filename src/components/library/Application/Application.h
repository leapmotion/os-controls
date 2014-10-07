#pragma once

#include "RenderableEventHandler.h"

// Interface class for top-level control of an application.
class Application : public RenderableEventHandler {
public:

  virtual ~Application () { }

  // One-time event methods.

  // Should set the video mode & take care of initial settings.  Errors should
  // be indicated by throwing an exception.
  virtual void Initialize () = 0;
  // Should clean up all resource & shut down the video context.  No exceptions should be thrown.
  virtual void Shutdown () = 0;

  // Accessor methods

  // The absolute application time, as tracked by the application.
  virtual TimePoint Time () const = 0;
};

// This is what drives the application, containing the game loop, event handling, etc.
// An example "main" function of the executable would instantiate an implementation
// of the Application interface, then call RunApplication on it, and that's it.
void RunApplication (Application &app);
