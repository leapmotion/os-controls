#include "SDLController.h"

#include <AppKit/NSOpenGL.h>
#include <AppKit/NSWindow.h>
#include <cstdint>
#include "SDL_syswm.h"

// NOTE: The reason this source file has to corresponding header file is because
// its contents are "private" to the library, meaning they don't need to be (and
// shouldn't be) make visible publicly to external users of this library.

// Sanity checks
#if !defined(__APPLE__)
#error "This file should only ever be compiled on Apple platforms, using the objective-c++ compiler"
#endif

#if !defined(SDL_VIDEO_DRIVER_COCOA)
#error "This file requires SDL to be built with SDL_VIDEO_DRIVER_COCOA"
#endif

void MakeTransparent_Apple (const SDL_SysWMinfo &sys_wm_info, SDL_GLContext c) {
  if (c == nullptr) {
    throw std::invalid_argument("there is no GL context to make transparent");
  }

  // Set the GL context opacity.
  {
    NSOpenGLContext *context = static_cast<NSOpenGLContext *>(c);
    // The opacity var should technically be a GLint, but from 
    // http://www.opengl.org/wiki/OpenGL_Type -- GLint is necessarily 32 bit,
    // so we can use a fixed int type without including any GL headers here.
    int32_t opacity = 0;
    [context setValues:&opacity forParameter:NSOpenGLCPSurfaceOpacity];
  }

  // Set window properties.
  {
    NSWindow *window = sys_wm_info.info.cocoa.window;
    // [window orderOut:window]; // this call seems to block the visibility of the window entirely
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
  }
}

