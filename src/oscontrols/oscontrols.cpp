#include "stdafx.h"
#include "interaction/GestureTriggerManifest.h"
#include "oscontrols.h"
#include "osinterface/AudioVolumeController.h"
#include "osinterface/LeapInput.h"
#include "osinterface/MediaController.h"
#include "utility/ComInitializer.h"
#if __APPLE__
#include <AppKit/NSColor.h>
#include <AppKit/NSOpenGL.h>
#include <AppKit/NSOpenGLView.h>
#include <AppKit/NSWindow.h>
#include <AppKit/NSView.h>
#include <OpenGL/GL.h>
#include <objc/runtime.h>
#endif

class GraphicsObject : public Object {
  public:
    GraphicsObject() : m_shape(100), m_time(0) {
      const auto radius = m_shape.getRadius();
      m_shape.setFillColor(sf::Color::Red);
      m_shape.setOrigin(radius,radius);
    }
    ~GraphicsObject() {}

    void AutoFilter(const OsControlRender& render) {
      const auto size = render.renderWindow->getSize();
      m_time += render.timeDelta.count();
      const double radius = static_cast<double>(m_shape.getRadius());
      const float x = static_cast<float>((size.x/2.0 - radius)*cos(m_time/4.0*M_PI));
      const float y = static_cast<float>((size.y/2.0 - radius)*sin(m_time/4.0*M_PI));
      m_shape.setPosition(size.x/2.0f + x, size.y/2.0f + y);
      render.renderWindow->draw(m_shape);
    }

  private:
    sf::CircleShape m_shape;
    double m_time;
};

//STUB IMPL!!!
class Drawable {
public:
  void Draw() {}
};

struct Scene {
  std::list<Drawable*> m_drawables;
};

int main(int argc, char **argv)
{
#if __APPLE__
  //
  // The isOpaque method in the SFOpenGLView class of SFML always returns YES
  // (as it just uses the default implementation of NSOpenGLView). This
  // causes us to always get an opaque view. We workaround this problem by
  // replacing that method with our own implementation that returns the
  // opaqueness based on the enclosing window, all thanks to the power of
  // Objective-C.
  //
  method_setImplementation(class_getInstanceMethod(NSClassFromString(@"SFOpenGLView"), @selector(isOpaque)),
                           imp_implementationWithBlock(^BOOL(id self, id arg) { return NO; }));
#endif
  ComInitializer initCom;
  AutoCurrentContext ctxt;
  ctxt->Initiate();

  try {
    AutoCreateContextT<OsControlContext> osCtxt;
    CurrentContextPusher pshr(osCtxt);
    AutoRequired<OsControl> control;
    AutoRequired<GraphicsObject> go;
    osCtxt->Initiate();
    control->Main();
  }
  catch (...) {}

  ctxt->SignalShutdown(true);
  return 0;
}

OsControl::OsControl(void) :
  m_mw(sf::VideoMode::getDesktopMode(),"Leap Os Control",sf::Style::None),
  m_bShouldStop(false),
  m_bRunning(false)
{
  MakeTransparent(m_mw);
}

void OsControl::MakeTransparent(const std::shared_ptr<sf::RenderWindow>& renderWindow) {
  if (!renderWindow)
    return;
  renderWindow->setVisible(false);
  sf::WindowHandle handle = renderWindow->getSystemHandle();
#if _WIN32
  HWND hWnd = static_cast<HWND>(handle);
  if (hWnd) {
    LONG flags = ::GetWindowLongA(hWnd, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT;
    if (m_Params.alwaysOnTop) {
      flags |= WS_EX_TOPMOST;
    }
    ::SetWindowLongA(hWnd, GWL_EXSTYLE, flags);
    ::SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
  } else {
    throw std::runtime_error("Error retrieving native window");
  }
  if (m_Params.alwaysOnTop) {
    ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
  }
  DWM_BLURBEHIND bb = { 0 };
  bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
  bb.fEnable = true;
  bb.hRgnBlur = CreateRectRgn(0, 0, 1, 1);
  ::DwmEnableBlurBehindWindow(hWnd, &bb);
#elif __APPLE__
  NSWindow* window = static_cast<NSWindow*>(handle);
  NSOpenGLView* view = [window contentView];

  if (!window || !view) {
    throw std::runtime_error("Error retrieving native window");
  }

  // Set the GL context opacity.
  NSOpenGLContext* context = [view openGLContext];
  // The opacity var should technically be a GLint, but from
  // http://www.opengl.org/wiki/OpenGL_Type -- GLint is necessarily 32 bit,
  // so we can use a fixed int type without including any GL headers here.
  int32_t opacity = 0;
  [context setValues:&opacity forParameter:NSOpenGLCPSurfaceOpacity];

  // Set window properties.
  [window setOpaque:NO];
  [window setHasShadow:NO];
  [window setHidesOnDeactivate:NO];
  [window setBackgroundColor:[NSColor clearColor]];
  [window setBackingType:NSBackingStoreBuffered];
  [window setSharingType:NSWindowSharingNone];
  [window setLevel:CGShieldingWindowLevel()];
  [window setCollectionBehavior:(NSWindowCollectionBehaviorCanJoinAllSpaces |
                                 NSWindowCollectionBehaviorStationary |
                                 NSWindowCollectionBehaviorFullScreenAuxiliary |
                                 NSWindowCollectionBehaviorIgnoresCycle)];
  [window display];
#else
  throw std::runtime_error("Missing transparent window implementation");
#endif
  renderWindow->setVisible(true);
}

void OsControl::Main(void) {
  GestureTriggerManifest manifest;

  auto clearOutstanding = MakeAtExit([this] {
    std::lock_guard<std::mutex> lk(m_lock);
    m_outstanding.reset();
    m_stateCondition.notify_all();
  });

  AutoRequired<AutoPacketFactory> factory;

  auto then = std::chrono::steady_clock::now();

  m_mw->setFramerateLimit(60);
  m_mw->setVerticalSyncEnabled(true);

  // Dispatch events until told to quit:
  while (!ShouldStop()) {
    sf::Event event;
    while (m_mw->pollEvent(event)) {
      HandleEvent(event);
    }

    // Active the window for OpenGL rendering
    m_mw->setActive();
    // Clear window
    m_mw->clear(sf::Color::Transparent);

    // Pilot a packet through the system:
    auto packet = factory->NewPacket();

    // Determine how long it has been since we were last here
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> timeDelta = now - then;
    then = now;

    // Have objects rendering into the specified window with the supplied change in time
    OsControlRender render = { m_mw, timeDelta };

    // Draw all of the objects
    if (packet->HasSubscribers(typeid(OsControlRender))) {
      packet->DecorateImmediate(render);
    }

    // Update the window
    m_mw->display();
  }
}

void OsControl::HandleEvent(const sf::Event& ev) const {
  switch (ev.type) {
  case sf::Event::Closed:
    m_mw->close();
    AutoCurrentContext()->SignalShutdown();
    break;
  default:
    break;
  }
}

void OsControl::Filter(void) {
  try {
    throw;
  }
  catch (std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
}

bool OsControl::Start(std::shared_ptr<Object> outstanding) {
  std::lock_guard<std::mutex> lk(m_lock);
  if (m_bShouldStop)
    return true;
  m_outstanding = outstanding;
  return true;
}

void OsControl::Wait(void) {
  std::unique_lock<std::mutex> lk(m_lock);
  m_stateCondition.wait(lk, [this] { return m_outstanding.get() == nullptr; });
}
