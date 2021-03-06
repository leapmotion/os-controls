#pragma once

#include "Application.h"
#include "GLController.h"
#include "RenderableEventHandler.h"
#include "SFMLController.h"
#include "OculusVR.h"
#include "Primitives.h"

#include <memory>
#include <vector>

// Interface class for top-level control of an application.
class OculusApplication : public Application {
public:

  OculusApplication();
  virtual ~OculusApplication();

  // Application interface methods.  See Application for comments and details.

  virtual void Initialize () override;
  virtual void Shutdown () override;

  virtual void Update (TimeDelta real_time_delta) override;
  virtual void Render (TimeDelta real_time_delta) const override;

  virtual EventHandlerAction HandleWindowEvent (const SDL_WindowEvent &ev) override;
  virtual EventHandlerAction HandleKeyboardEvent (const SDL_KeyboardEvent &ev) override;
  virtual EventHandlerAction HandleMouseMotionEvent (const SDL_MouseMotionEvent &ev) override;
  virtual EventHandlerAction HandleMouseButtonEvent (const SDL_MouseButtonEvent &ev) override;
  virtual EventHandlerAction HandleMouseWheelEvent (const SDL_MouseWheelEvent &ev) override;
  virtual EventHandlerAction HandleQuitEvent (const SDL_QuitEvent &ev) override;
  virtual EventHandlerAction HandleGenericSDLEvent (const SDL_Event &ev) override;

  virtual TimePoint Time () const override;

  // Other methods go below here.

private:

  template <typename EventType_>
  EventHandlerAction DispatchEventToApplicationLayers (
    const EventType_ &ev,
    EventHandlerAction (EventHandler::*HandleEvent)(const EventType_ &))
  {
    // Give each application layer a chance to handle the event, stopping if one returns EventHandlerAction::CONSUME.
    for (auto it = m_applicationLayers.rbegin(); it != m_applicationLayers.rend(); ++it) {
      RenderableEventHandler &layer = **it;
      if ((layer.*HandleEvent)(ev) == EventHandlerAction::CONSUME)
        return EventHandlerAction::CONSUME;
    }
    // No layers consumed the event, so pass it on.
    return EventHandlerAction::PASS_ON;
  }

  mutable OculusVR m_Oculus;
  SFMLController m_SFMLController;
  GLController m_GLController;
  TimePoint m_applicationTime;
  std::vector<std::shared_ptr<RenderableEventHandler>> m_applicationLayers;
};
