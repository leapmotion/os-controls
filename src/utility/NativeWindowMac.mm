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
