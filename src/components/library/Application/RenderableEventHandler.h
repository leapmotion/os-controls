#pragma once

#include "EventHandler.h"
#include "Renderable.h"

// Interface class for a thing that can be rendered and can handle events.  For example, the
// particular "layers" of an application (e.g. 3d scene, GUI layer, title screen) would implement
// this.
class RenderableEventHandler : public EventHandler, public Renderable {
public:

  virtual ~RenderableEventHandler () { }
};
