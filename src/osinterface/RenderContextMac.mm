#include "RenderContextMac.h"

#include <OpenGL/gl.h>
#include <Cocoa/Cocoa.h>

//
// RenderContext
//

RenderContext* RenderContext::New(void) {
  return new RenderContextMac(std::static_pointer_cast<RenderContextMac>(GetRootContext()));
}

RenderContext* RenderContext::Root(void) {
  return new RenderContextMac(std::shared_ptr<RenderContextMac>());
}

//
// RenderContextMac
//

RenderContextMac::RenderContextMac(std::shared_ptr<RenderContextMac> rootContext) :
  RenderContext(std::move(rootContext)),
  m_context(nullptr),
  m_rootWindow(nullptr)
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
  NSOpenGLContext* shareContext = m_rootRenderContext ?
      reinterpret_cast<NSOpenGLContext*>(std::static_pointer_cast<RenderContextMac>(m_rootRenderContext)->m_context) : nil;
  NSOpenGLContext* context = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:shareContext];
  [pixelFormat release];
  m_context = reinterpret_cast<void*>(context);

  // If we are creating the root context, we need to create a window to accompany this context
  if (!m_rootRenderContext) {
    NSWindow* rootWindow = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 1, 1)
                                                       styleMask:NSBorderlessWindowMask
                                                         backing:NSBackingStoreBuffered
                                                           defer:NO];
    NSOpenGLView* rootView = [[NSOpenGLView alloc] initWithFrame:[rootWindow frame]];
    [rootWindow setContentView:rootView];
    [rootView setOpenGLContext:context];
    [context setView:rootView];
    [rootView release];
    m_rootWindow = reinterpret_cast<void*>(rootWindow);

    SetActive(true);
  }
}

RenderContextMac::~RenderContextMac(void)
{
  SetActive(false);

  NSOpenGLContext* context = reinterpret_cast<NSOpenGLContext*>(m_context);
  [context clearDrawable];
  [context release];

  NSWindow* rootWindow = reinterpret_cast<NSWindow*>(m_rootWindow);
  [rootWindow release];
}

void RenderContextMac::SetActive(bool active)
{
  NSOpenGLContext* context = reinterpret_cast<NSOpenGLContext*>(m_context);
  if (active) {
    [context makeCurrentContext];
  } else if (m_rootRenderContext) {
    m_rootRenderContext->SetActive(true);
  } else {
    [NSOpenGLContext clearCurrentContext];
  }
}

void RenderContextMac::FlushBuffer(void)
{
  NSOpenGLContext* context = reinterpret_cast<NSOpenGLContext*>(m_context);
  if (context == [NSOpenGLContext currentContext]) {
    [context flushBuffer];
  }
}
