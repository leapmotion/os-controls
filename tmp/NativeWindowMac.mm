#include "stdafx.h"
#include "NativeWindow.h"

#include <AppKit/NSColor.h>
#include <AppKit/NSOpenGL.h>
#include <AppKit/NSOpenGLView.h>
#include <AppKit/NSWindow.h>
#include <AppKit/NSView.h>
#include <OpenGL/GL.h>

void NativeWindow::MakeTransparent(const Handle& handle) {
  const NSWindow* window = static_cast<const NSWindow*>(handle);
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
  [context setValues : &opacity forParameter : NSOpenGLCPSurfaceOpacity];

  // Set window properties.
  [window setOpaque : NO];
  [window setHasShadow : NO];
  [window setHidesOnDeactivate : NO];
  [window setBackgroundColor : [NSColor clearColor]];
  [window setBackingType : NSBackingStoreBuffered];
  [window setSharingType : NSWindowSharingNone];
  [window setCollectionBehavior : (NSWindowCollectionBehaviorCanJoinAllSpaces |
    NSWindowCollectionBehaviorStationary |
    NSWindowCollectionBehaviorFullScreenAuxiliary |
    NSWindowCollectionBehaviorIgnoresCycle)];
  [view setWantsBestResolutionOpenGLSurface:YES];
  [window display];
}

void NativeWindow::MakeAlwaysOnTop(const Handle& handle) {
  const NSWindow* window = static_cast<const NSWindow*>(handle);
  if (!window) {
    throw std::runtime_error("Error retrieving native window");
  }
  [window setLevel : CGShieldingWindowLevel()];
}

void NativeWindow::AllowInput(const Handle& handle, bool allowInput) {
  const NSWindow* window = static_cast<const NSWindow*>(handle);
  if (!window) {
    throw std::runtime_error("Error retrieving native window");
  }
  [window setAcceptsMouseMovedEvents:allowInput];
  [window setIgnoresMouseEvents:!allowInput];
}

extern "C" {

typedef int CGSConnection;
typedef int CGSWindow;

extern CGSConnection CGSMainConnectionID(void);
extern CGError CGSGetConnectionIDForPSN(CGSConnection cid, const ProcessSerialNumber* psn, CGSConnection* outOwnerCID);
extern CGError CGSSetFrontWindow(const CGSConnection cid, const CGSWindow wid);
extern CGError CGSReleaseConnection(CGSConnection cid);

typedef enum _CGSWindowOrderingMode {
  kCGSOrderBelow = -1,
  kCGSOrderOut   =  0,
  kCGSOrderAbove =  1
} CGSWindowOrderingMode;

extern CGError CGSOrderWindow(const CGSConnection cid, const CGSWindow wid, CGSWindowOrderingMode place, CGSWindow relativeToWindowID);
extern CGError CGSMoveWindow(const CGSConnection cid, CGSWindow wid, CGPoint *aPoint);

}

#include <iostream>

void NativeWindow::RaiseWindowAtPosition(float x, float y) {
  NSPoint point{x, y};
  @autoreleasepool {
    const CGSWindow windowID = (CGSWindow) [NSWindow windowNumberAtPoint:point belowWindowWithWindowNumber:0];
    std::cerr << "WindowID: " << windowID << "\n";
    if (!windowID) {
      return;
    }
    const NSArray* windowArray =
      (id)CFBridgingRelease(CGWindowListCopyWindowInfo(kCGWindowListOptionIncludingWindow, windowID));
    if ([windowArray count] <= 0) {
      return;
    }
    const NSDictionary* info = [windowArray objectAtIndex:0];
    if (info == nil) {
      return;
    }
    const pid_t pid = [[info objectForKey:(id)kCGWindowOwnerPID] intValue];
    ProcessSerialNumber psn{0, 0};
    CGSConnection cid = 0;
    if (GetProcessForPID(pid, &psn) != noErr) {
      return;
    }
    if (CGSGetConnectionIDForPSN(CGSMainConnectionID(), &psn, &cid) != kCGErrorSuccess) {
      return;
    }
    std::cerr << CGSSetFrontWindow(cid, windowID) << "\n";
    CGSReleaseConnection(cid);
  }
}
