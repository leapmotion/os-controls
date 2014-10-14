#include "stdafx.h"
#include "RenderContextWin.h"

#include <GL/wglew.h>
#include <GL/gl.h>

//
// RenderContext
//

RenderContext* RenderContext::New(void) {
  return new RenderContextWin(std::static_pointer_cast<RenderContextWin>(GetRootContext()));
}

RenderContext* RenderContext::Root(void) {
  return new RenderContextWin(std::shared_ptr<RenderContextWin>());
}

//
// RenderContextWin
//

RenderContextWin* RenderContextWin::New(HWND hWnd) {
  return new RenderContextWin(std::static_pointer_cast<RenderContextWin>(GetRootContext()), hWnd);
}

RenderContextWin::RenderContextWin(std::shared_ptr<RenderContextWin> rootContext, HWND hWnd) :
  RenderContext(std::move(rootContext)),
  m_context(nullptr),
  m_hWnd(nullptr),
  m_dc(nullptr)
{
  if (hWnd) {
    m_hWnd = hWnd;
  } else {
    m_hWnd = ::CreateWindowA("RenderContextWinRoot", "", WS_POPUP | WS_DISABLED, 0, 0, 1, 1,
                             nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    if (!m_hWnd) {
      return;
    }
    ::ShowWindow(m_hWnd, SW_HIDE);
  }
  m_dc = ::GetDC(m_hWnd);
  if (!m_dc) {
    return;
  }

  PIXELFORMATDESCRIPTOR pfd;
  int pixelFormat = 0;

  if (hWnd && m_rootRenderContext) {
    PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB =
        reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(::wglGetProcAddress("wglChoosePixelFormatARB"));
    if (wglChoosePixelFormatARB) {
      const float floatAttrs[] = {0, 0};
      const int SAMPLES_INDEX = 1; // Index into intAttrs where we store the samples
      int intAttrs[] = {
        WGL_SAMPLES_ARB,        16, // This position MUST match location specified by SAMPLES_INDEX
        WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
        WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,     32,
        WGL_RED_BITS_ARB,       8,
        WGL_GREEN_BITS_ARB,     8,
        WGL_BLUE_BITS_ARB,      8,
        WGL_ALPHA_BITS_ARB,     8,
        WGL_DEPTH_BITS_ARB,     0,
        WGL_STENCIL_BITS_ARB,   0,
        0,                      0
      };
      UINT numFormats = 0;
      // Search for a valid pixel format
      while (intAttrs[1] > 0 &&
             (!wglChoosePixelFormatARB(m_dc, intAttrs, floatAttrs, 1, &pixelFormat, &numFormats) ||
              numFormats == 0)) {
        intAttrs[SAMPLES_INDEX]--;
      }
      if (pixelFormat) {
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        if (!::DescribePixelFormat(m_dc, pixelFormat, sizeof(pfd), &pfd)) {
          pixelFormat = 0; // Something when wrong!
        }
      }
    }
  }
  if (!pixelFormat) {
    ::ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags =  PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA,
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    pixelFormat = ::ChoosePixelFormat(m_dc, &pfd);
    if (!pixelFormat) {
      return; // Failed to find a matching pixel format
    }
  }

  if (!::SetPixelFormat(m_dc, pixelFormat, &pfd)) {
    return; // Failed to set the pixel format
  }

  HGLRC shareContext =
      m_rootRenderContext ? std::static_pointer_cast<RenderContextWin>(m_rootRenderContext)->m_context : nullptr;

  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
      reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(::wglGetProcAddress("wglCreateContextAttribsARB"));
  if (wglCreateContextAttribsARB) {
    const int MAJOR_VERSION_INDEX = 1; // Index into attrs where we store the major version
    const int MINOR_VERSION_INDEX = 3; // Index into attrs where we store the minor version
    int attrs[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 5, // This position MUST match location specified by MAJOR_VERSION_INDEX
      WGL_CONTEXT_MINOR_VERSION_ARB, 0, // This position MUST match location specified by MINOR_VERSION_INDEX
      WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
      0,                             0
    };
    do {
      m_context = wglCreateContextAttribsARB(m_dc, shareContext, attrs);
      if (!m_context) {
        // Decrement the major.minor version until we have a match (or we go below version 3.0)
        if (attrs[MINOR_VERSION_INDEX] > 0) {
          attrs[MINOR_VERSION_INDEX]--;
        } else {
          attrs[MAJOR_VERSION_INDEX]--;
          attrs[MINOR_VERSION_INDEX] = 9;
        }
      }
    } while (!m_context && attrs[MAJOR_VERSION_INDEX] >= 3);
  }

  if (!m_context) {
    m_context = ::wglCreateContext(m_dc);
    if (m_context && shareContext) {
      ::wglShareLists(shareContext, m_context);
    }
  }
  if (!shareContext) {
    SetActive(true);
  }
}

RenderContextWin::~RenderContextWin(void)
{
  if (m_context) {
    SetActive(false);
    ::wglDeleteContext(m_context);
    m_context = nullptr;
  }
  if (m_hWnd) {
    if (m_dc) {
      ::ReleaseDC(m_hWnd, m_dc);
      m_dc = nullptr;
    }
    ::DestroyWindow(m_hWnd);
    m_hWnd = nullptr;
  }
}

void RenderContextWin::SetActive(bool active)
{
  if (!m_context || !m_dc) {
    return;
  }
  if (active) {
    ::wglMakeCurrent(m_dc, m_context);
  } else if (m_rootRenderContext) {
    m_rootRenderContext->SetActive(true);
  } else {
    ::wglMakeCurrent(nullptr, nullptr);
  }
}

void RenderContextWin::FlushBuffer(void)
{
  if (!m_context || !m_dc) {
    return;
  }
  ::SwapBuffers(m_dc);
}
