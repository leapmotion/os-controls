#include "stdafx.h"
#include "NativeWindow.h"

#include <AppKit/NSColor.h>
#include <AppKit/NSOpenGL.h>
#include <AppKit/NSOpenGLView.h>
#include <AppKit/NSWindow.h>
#include <AppKit/NSView.h>
#include <OpenGL/GL.h>
#include <objc/runtime.h>

void NativeWindow::AllowTransparency() {
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
}

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
  [window setCollectionBehavior : (NSWindowCollectionBehaviorCanJoinAllSpaces |
    NSWindowCollectionBehaviorStationary |
    NSWindowCollectionBehaviorFullScreenAuxiliary |
    NSWindowCollectionBehaviorIgnoresCycle)];
  [view setWantsBestResolutionOpenGLSurface:YES];
  [window display];
}

void NativeWindow::MakeAlwaysOnTop(const Handle handle) {
  NSWindow* window = static_cast<NSWindow*>(handle);
  if (!window) {
    throw std::runtime_error("Error retrieving native window");
  }
  [window setLevel : CGShieldingWindowLevel()];
}
