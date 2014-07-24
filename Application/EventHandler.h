#pragma once

#if _WIN32
#include "SDL.h"
#include "SDL_opengl.h"
#else
#include "SDL.h"
#include "SDL_opengl.h"
#endif
#undef main

// Possible actions for event handlers -- CONSUME indicates lower-priority event
// handlers should not get a chance to process the event.  PASS_ON indicates otherwise.
// The default implementation of an event handling method is to do nothing and PASS_ON.
enum class EventHandlerAction { CONSUME, PASS_ON };

// Interface class for a thing that needs to handle events.
class EventHandler {
public:

  virtual ~EventHandler () { }

  // Event handling methods.  The type-specific event handler will be called unless it doesn't
  // fall into one of the specific categories, in which case, HandleGenericEvent will be called.

  virtual EventHandlerAction HandleWindowEvent (const SDL_WindowEvent &ev) { return EventHandlerAction::PASS_ON; }
  virtual EventHandlerAction HandleKeyboardEvent (const SDL_KeyboardEvent &ev) { return EventHandlerAction::PASS_ON; }
  virtual EventHandlerAction HandleMouseMotionEvent (const SDL_MouseMotionEvent &ev) { return EventHandlerAction::PASS_ON; }
  virtual EventHandlerAction HandleMouseButtonEvent (const SDL_MouseButtonEvent &ev) { return EventHandlerAction::PASS_ON; }
  virtual EventHandlerAction HandleMouseWheelEvent (const SDL_MouseWheelEvent &ev) { return EventHandlerAction::PASS_ON; }
  virtual EventHandlerAction HandleQuitEvent (const SDL_QuitEvent &ev) { return EventHandlerAction::PASS_ON; }
  virtual EventHandlerAction HandleGenericSDLEvent (const SDL_Event &ev) { return EventHandlerAction::PASS_ON; } ///< Fallback event handler for when the event didn't fall into one of the other type-specific categories.
};
