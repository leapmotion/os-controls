#include "stdafx.h"
#include "NativeWindow.h"

#include <AppKit/NSColor.h>
#include <AppKit/NSOpenGL.h>
#include <AppKit/NSOpenGLView.h>
#include <AppKit/NSWindow.h>
#include <AppKit/NSView.h>
#include <OpenGL/GL.h>
#include <objc/runtime.h>

void NativeWindow::MakeTransparent(const Handle handle) {
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
  [context setValues : &opacity forParameter : NSOpenGLCPSurfaceOpacity];

  // Set window properties.
  [window setOpaque : NO];
  [window setHasShadow : NO];
  [window setHidesOnDeactivate : NO];
  [window setBackgroundColor : [NSColor clearColor]];
  [window setBackingType : NSBackingStoreBuffered];
  [window setSharingType : NSWindowSharingNone];
  [window setLevel : CGShieldingWindowLevel()];
  [window setCollectionBehavior : (NSWindowCollectionBehaviorCanJoinAllSpaces |
    NSWindowCollectionBehaviorStationary |
    NSWindowCollectionBehaviorFullScreenAuxiliary |
    NSWindowCollectionBehaviorIgnoresCycle)];
  [window display];
}

void NativeWindow::MakeAlwaysOnTop(const Handle handle) {
  static_assert("Jon, please implement me!");
}