#include "Leap/GL/Internal/RenderBuffer.h"

#include "Leap/GL/Error.h"

namespace Leap {
namespace GL {
// The contents of the Internal namespace are not intended to be used publicly, and provide
// no guarantee as to the stability of their API.  The classes and functions are used
// internally in the implementation of the publicly-presented classes.
namespace Internal {

// static

RenderBuffer* RenderBuffer::Create(int width, int height, const Format& format)
{
  return new RenderBuffer(width, height, format);
}

// public

void RenderBuffer::Bind() const
{
  glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferId);
  WarnUponGLError("in glBindRenderbuffer");
}

GLuint RenderBuffer::Id() const
{
  return m_RenderBufferId;
}

void RenderBuffer::Unbind() const
{
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  WarnUponGLError("in glBindRenderbuffer");
}

// protected

RenderBuffer::RenderBuffer(int width, int height, const Format& format)
  : m_Width(width)
  , m_Height(height)
{
  // TODO: make this a util function
  GLint maxSamples;
  glGetIntegerv(GL_MAX_SAMPLES_EXT, &maxSamples);
  WarnUponGLError("in glGetIntegerv");
  
  if (format.samples > maxSamples) {
    m_Samples = maxSamples;
  } else {
    m_Samples = format.samples;
  }

  glGenRenderbuffers(1, &m_RenderBufferId);
  WarnUponGLError("in glGenRenderbuffers");
  
  Bind();

  if (m_Samples == 0) {
    glRenderbufferStorage(GL_RENDERBUFFER, format.internalFormat, m_Width, m_Height);
    WarnUponGLError("in glRenderbufferStorage");
  } else {
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Samples, format.internalFormat, m_Width, m_Height);
    WarnUponGLError("in glRenderbufferStorageMultisample");
  }

  // TODO: check QT error

  Unbind();
}

} // end of namespace Internal
} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
