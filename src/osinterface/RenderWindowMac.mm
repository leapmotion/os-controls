#include "RenderWindowMac.h"

#include <OpenGL/gl.h>
#include <Cocoa/Cocoa.h>

//
// RenderWindowOpenGLView
//

@interface RenderWindowOpenGLView : NSOpenGLView {
  bool m_isShown;
  NSOpenGLContext* m_previousContext;
  RenderWindowMac* m_owner;
}
-(id) initWithFrame:(NSRect)frame pixelFormat:(NSOpenGLPixelFormat*)format owner:(RenderWindowMac*)owner;
-(void) dealloc;
-(void) receiveSleepNotification:(NSNotification*)notification;
-(void) receiveWakeNotification:(NSNotification*)notification;
-(void) updateContext;
-(void) setActive:(BOOL)isActive;
-(void) flushBuffer;
-(BOOL) isOpaque;
@end

@implementation RenderWindowOpenGLView
-(id) initWithFrame:(NSRect)frame pixelFormat:(NSOpenGLPixelFormat*)format owner:(RenderWindowMac*)owner;
{
  if (!owner) {
    return nil;
  }
  self = [super initWithFrame:frame pixelFormat:format];
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
    GLint interval = 1;
    [[self openGLContext] setValues:&interval forParameter:NSOpenGLCPSwapInterval];
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

-(void) updateContext
{
  GLint opacity = m_owner->IsTransparent() ? 0 : 1;
  [[self openGLContext] setValues:&opacity forParameter:NSOpenGLCPSurfaceOpacity];
}

-(void) setActive:(BOOL)isActive
{
  if (isActive) {
    if (m_previousContext == nil) {
      m_previousContext = [NSOpenGLContext currentContext];
      [[self openGLContext] makeCurrentContext];
    } else {
      // Unmatched setActive (this should never happen)-- FIXME
    }
  } else if (m_previousContext != nil) {
    [m_previousContext makeCurrentContext];
    m_previousContext = nil;
  } else {
    // Unmatched setActive (this should never happen)-- FIXME
  }
}

-(void) flushBuffer
{
  if (m_previousContext != nil)  {
    [[self openGLContext] flushBuffer];
  } else {
    // Called when not active (this should never happen)-- FIXME
  }
}

-(BOOL) isOpaque
{
  return !m_owner->IsTransparent();
}
@end

//
// RenderWindow
//

RenderWindow* RenderWindow::New(void) {
  return new RenderWindowMac;
}

//
// RenderWindowMac
//

RenderWindowMac::RenderWindowMac(void):
  m_window(nullptr),
  m_mainDisplayHeight(CGDisplayPixelsHigh(CGMainDisplayID()))
{
  const NSOpenGLPixelFormatAttribute attrs[] = {
    NSOpenGLPFAAccelerated,
    NSOpenGLPFAMultisample,
    NSOpenGLPFASampleBuffers, static_cast<NSOpenGLPixelFormatAttribute>(1),
    NSOpenGLPFASamples, static_cast<NSOpenGLPixelFormatAttribute>(16),
    NSOpenGLPFADepthSize, static_cast<NSOpenGLPixelFormatAttribute>(0),
    NSOpenGLPFAStencilSize, static_cast<NSOpenGLPixelFormatAttribute>(0),
    NSOpenGLPFAAlphaSize, static_cast<NSOpenGLPixelFormatAttribute>(8),
    NSOpenGLPFAClosestPolicy,
    NSOpenGLPFADoubleBuffer,
    static_cast<NSOpenGLPixelFormatAttribute>(0)
  };

  NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
  RenderWindowOpenGLView *openGLView = [[RenderWindowOpenGLView alloc] initWithFrame:CGRectZero pixelFormat:pixelFormat owner:this];
  NSWindow* window = [[NSWindow alloc] init];

  [window setContentView:openGLView];
  [openGLView release];

  AllowInput(m_allowInput);
  SetTransparent(m_isTransparent);

  [window setHasShadow:NO];
  [window setHidesOnDeactivate:NO];
  [window setStyleMask:NSBorderlessWindowMask];
  [window setBackgroundColor:[NSColor clearColor]];
  [window setBackingType:NSBackingStoreBuffered];
  [window setSharingType:NSWindowSharingNone];
  [window setLevel:CGShieldingWindowLevel()];
  [window setCollectionBehavior:(NSWindowCollectionBehaviorCanJoinAllSpaces |
                                 NSWindowCollectionBehaviorStationary |
                                 NSWindowCollectionBehaviorFullScreenAuxiliary |
                                 NSWindowCollectionBehaviorIgnoresCycle)];
  [pixelFormat release];

  m_window = reinterpret_cast<void*>(window);
}

RenderWindowMac::~RenderWindowMac()
{
  SetVisible(false);
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  [window release];
}

OSPoint RenderWindowMac::Postion() const
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  NSRect frame = [window frame];
  return AdjustCoordinates(frame).origin;
}

OSSize RenderWindowMac::Size() const
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  NSRect frame = [window frame];
  return frame.size;
}

OSRect RenderWindowMac::Rect() const
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  NSRect frame = [window frame];
  return AdjustCoordinates(frame);
}

void RenderWindowMac::SetPosition(const OSPoint& position)
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  NSRect frame = [window frame];
  frame.origin = position;
  [window setFrameOrigin:AdjustCoordinates(frame).origin];
}

void RenderWindowMac::SetSize(const OSSize& size)
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  NSRect frame = [window frame];
  frame.size = size;
  [window setFrame:frame display:NO];
}

void RenderWindowMac::SetRect(const OSRect& rect)
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  [window setFrame:AdjustCoordinates(rect) display:NO];
}

void RenderWindowMac::SetTransparent(bool transparent)
{
  m_isTransparent = transparent;
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  [window setOpaque:!m_isTransparent];
  [[window contentView] updateContext];
}

void RenderWindowMac::AllowInput(bool allowInput)
{
  m_allowInput = allowInput;
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  [window setAcceptsMouseMovedEvents:(m_allowInput ? YES : NO)];
  [window setIgnoresMouseEvents:(m_allowInput ? NO : YES)];
}

void RenderWindowMac::SetVisible(bool visible)
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  if (visible) {
    [window orderFrontRegardless];
  } else {
    [window orderOut:window];
  }
}

void RenderWindowMac::SetActive(bool active)
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  [[window contentView] setActive:(active ? YES : NO)];
}

void RenderWindowMac::Display(void)
{
  NSWindow* window = reinterpret_cast<NSWindow*>(m_window);
  [[window contentView] flushBuffer];
}

void RenderWindowMac::OnScreenSizeChange(void)
{
  m_mainDisplayHeight = CGDisplayPixelsHigh(CGMainDisplayID());
}
