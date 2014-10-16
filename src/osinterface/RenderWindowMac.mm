#include "RenderWindowMac.h"
#include "RenderContextMac.h"

#include <OpenGL/gl.h>
#include <Cocoa/Cocoa.h>

//
// RenderWindowOpenGLView
//

@interface RenderWindowOpenGLView : NSOpenGLView {
  bool m_isShown;
  RenderWindowMac* m_owner;
}
-(id) initWithFrame:(NSRect)frame owner:(RenderWindowMac*)owner;
-(void) dealloc;
-(void) receiveSleepNotification:(NSNotification*)notification;
-(void) receiveWakeNotification:(NSNotification*)notification;
-(void) update;
-(void) updateContextSettings;
-(BOOL) isOpaque;
@end

@implementation RenderWindowOpenGLView
-(id) initWithFrame:(NSRect)frame owner:(RenderWindowMac*)owner;
{
  if (!owner) {
    return nil;
  }
  self = [super initWithFrame:frame];
  if (self) {
    @autoreleasepool {
      NSWorkspace* workspace = [NSWorkspace sharedWorkspace];

      [[workspace notificationCenter] addObserver:self
                                         selector:@selector(receiveSleepNotification:)
                                             name:NSWorkspaceWillSleepNotification
                                           object:nil];
      [[workspace notificationCenter] addObserver:self
                                         selector:@selector(receiveWakeNotification:)
                                             name:NSWorkspaceDidWakeNotification
                                           object:nil];
    }
    [self prepareOpenGL];
    m_isShown = false;
    m_owner = owner;
  }
  return self;
}

-(void) dealloc
{
  @autoreleasepool {
    [[[NSWorkspace sharedWorkspace] notificationCenter] removeObserver:self];
  }
  [[self openGLContext] clearDrawable];
  [self clearGLContext];
  [super dealloc];
}

-(void) receiveSleepNotification:(NSNotification*)notification
{
  m_isShown = [[self window] isVisible];
  if (m_isShown) {
    m_owner->SetVisible(false);
  }
}

-(void) receiveWakeNotification:(NSNotification*)notification
{
  if (m_isShown) {
    m_owner->SetVisible(true);
  }
}

-(void) update
{
  [[self openGLContext] update];
}

-(void) updateContextSettings
{
  GLint opacity = m_owner->IsTransparent() ? 0 : 1;
  GLint interval = m_owner->UseVSync() ? 1 : 0;
  NSOpenGLContext* context = [self openGLContext];
  [context setValues:&opacity forParameter:NSOpenGLCPSurfaceOpacity];
  [context setValues:&interval forParameter:NSOpenGLCPSwapInterval];
}

-(BOOL) isOpaque
{
  return !m_owner->IsTransparent();
}
@end

//
// RenderWindow
//

RenderWindow* RenderWindow::New(void)
{
  return new RenderWindowMac;
}

//
// RenderWindowMac
//

RenderWindowMac::RenderWindowMac(void):
  m_renderContext(static_cast<RenderContextMac*>(RenderContext::New())),
  m_window(nullptr),
  m_mainDisplayHeight(CGDisplayPixelsHigh(CGMainDisplayID()))
{
  NSWindow* window = [[NSWindow alloc] initWithContentRect:AdjustCoordinates(NSMakeRect(0, 0, 1, 1))
                                                 styleMask:NSBorderlessWindowMask
                                                   backing:NSBackingStoreBuffered
                                                     defer:NO];

  RenderWindowOpenGLView *openGLView = [[RenderWindowOpenGLView alloc] initWithFrame:[window frame] owner:this];
  [window setContentView:openGLView];
  NSOpenGLContext* context = reinterpret_cast<NSOpenGLContext*>(m_renderContext->GetNativeContext());
  [openGLView setOpenGLContext:context];
  [context setView:openGLView];
  [openGLView release];

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

  m_window = reinterpret_cast<void*>(window);

  SetTransparent(m_isTransparent);
}

RenderWindowMac::~RenderWindowMac()
{
  SetVisible(false);
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  [window release];
}

OSPoint RenderWindowMac::GetPostion() const
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  return AdjustCoordinates([window frame]).origin;
}

OSSize RenderWindowMac::GetSize() const
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  return [window frame].size;
}

OSRect RenderWindowMac::GetRect() const
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  return AdjustCoordinates([window frame]);
}

void RenderWindowMac::SetPosition(const OSPoint& position)
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  NSRect frame = AdjustCoordinates([window frame]);
  frame.origin = position;
  [window setFrameOrigin:AdjustCoordinates(frame).origin];
}

void RenderWindowMac::SetSize(const OSSize& size)
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  NSRect frame = AdjustCoordinates([window frame]);
  frame.size = size;
  [window setFrame:AdjustCoordinates(frame) display:YES];
}

void RenderWindowMac::SetRect(const OSRect& rect)
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  [window setFrame:AdjustCoordinates(rect) display:YES];
}

void RenderWindowMac::SetVSync(bool vsync)
{
  m_useVSync = vsync;
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  [[window contentView] updateContextSettings];
}

void RenderWindowMac::SetTransparent(bool transparent)
{
  m_isTransparent = transparent;
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  [window setOpaque:(m_isTransparent ? NO : YES)];
  [window setAcceptsMouseMovedEvents:(m_isTransparent ? NO : YES)];
  [window setIgnoresMouseEvents:(m_isTransparent ? YES : NO)];
  [[window contentView] updateContextSettings];
}

void RenderWindowMac::SetVisible(bool visible)
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  m_isVisible = visible;
  if (visible) {
    [window orderFrontRegardless];
  } else {
    [window orderOut:window];
  }
}

void RenderWindowMac::SetActive(bool active)
{
  if (!m_renderContext) {
    return;
  }
  m_renderContext->SetActive(active);
}

void RenderWindowMac::FlushBuffer(void)
{
  if (!m_renderContext) {
    return;
  }
  m_renderContext->FlushBuffer();
}

void RenderWindowMac::ProcessEvents(void)
{
  NSEvent* event = nil;

  @autoreleasepool {
    while ((event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                       untilDate:[NSDate distantPast]
                                          inMode:NSDefaultRunLoopMode
                                         dequeue:YES])) {
      [NSApp sendEvent:event];
    }
  }
}

void RenderWindowMac::OnScreenSizeChange(void)
{
  m_mainDisplayHeight = CGDisplayPixelsHigh(CGMainDisplayID());
}
