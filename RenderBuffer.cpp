#include "RenderBuffer.h"

// static

RenderBuffer* RenderBuffer::Create(int width, int height, const Format& format)
{
  return new RenderBuffer(width, height, format);
}

// public

void RenderBuffer::Bind() const
{
  glBindRenderbuffer(GL_RENDERBUFFER, m_RenderBufferId);
}

GLuint RenderBuffer::Id() const
{
  return m_RenderBufferId;
}

void RenderBuffer::Unbind() const
{
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

// protected

RenderBuffer::RenderBuffer(int width, int height, const Format& format)
  : m_Width(width)
  , m_Height(height)
{
  // TODO: make this a util function
  GLint maxSamples;
  glGetIntegerv(GL_MAX_SAMPLES_EXT, &maxSamples);
  if (format.samples > maxSamples) {
    m_Samples = maxSamples;
  } else {
    m_Samples = format.samples;
  }

  glGenRenderbuffers(1, &m_RenderBufferId);
  Bind();

  if (m_Samples == 0) {
    glRenderbufferStorage(GL_RENDERBUFFER, format.internalFormat, m_Width, m_Height);
  } else {
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Samples, format.internalFormat, m_Width, m_Height);
  }

  // TODO: check QT error

  Unbind();
}
