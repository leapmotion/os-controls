#pragma once

#include "SDL.h"

typedef double TimePoint; // TODO: change to std::chrono::time_point once C++11 is fully used.
typedef double TimeDelta; // TODO: change to std::chrono::duration once C++11 is fully used.

// Interface class for a thing that can be rendered (and has updateable state).
class Renderable {
public:

  virtual ~Renderable () { }

  // Updates application state.  realTimeDelta is in seconds.
  // The application may scale this delta as desired.
  virtual void Update (TimeDelta realTimeDelta) = 0;
  // Renders the app.  realTimeDelta is in seconds.  The
  // application may scale this delta as desired.
  virtual void Render (TimeDelta realTimeDelta) const = 0;
};
