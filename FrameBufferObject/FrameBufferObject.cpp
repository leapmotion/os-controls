#include "FrameBufferObject.h"

#include "GLError.h"
#include "GLTexture2.h"
#include "RenderBuffer.h"

#include <assert.h>

// static

FrameBufferObject* FrameBufferObject::Create(int width, int height, const Format& format)
{
  FrameBufferObject* framebufferObject = new FrameBufferObject(width, height, format);

  if (framebufferObject && !framebufferObject->HasError() && framebufferObject->Status() == GL_FRAMEBUFFER_COMPLETE) {
    return framebufferObject;
  }
  
  delete framebufferObject;
  return nullptr;
}

void FrameBufferObject::Blit(FrameBufferObject* source, FrameBufferObject* target, GLbitfield buffers, GLenum filter)
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER_EXT, source->Id());
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER_EXT, target->Id());
  
  glBlitFramebuffer(0, 0, source->Width(), source->Height(),
                    0, 0, target->Width(), target->Height(), buffers, filter);
  GLWarnUponError("in glBlitFramebuffer");
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// public

FrameBufferObject::~FrameBufferObject()
{
  if (m_ColorTexture) {
    delete m_ColorTexture;
  }
  
  if (m_ColorRenderBuffer) {
    delete m_ColorRenderBuffer;
  }

  if (m_DepthTexture) {
    delete m_DepthTexture;
  }
  
  if (m_DepthRenderBuffer) {
    delete m_DepthRenderBuffer;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glDeleteFramebuffers(1, &m_FramebufferId);
}

void FrameBufferObject::Bind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferId);
}

GLTexture2* FrameBufferObject::ColorTexture()
{
  return m_ColorTexture;
}

bool FrameBufferObject::HasError() const
{
  return m_Error;
}

int FrameBufferObject::Height() const
{
  return m_Height;
}

GLuint FrameBufferObject::Id() const
{
  return m_FramebufferId;
}

int FrameBufferObject::Width() const
{
  return m_Width;
}

GLenum FrameBufferObject::Status() const
{
  return m_Status;
}

void FrameBufferObject::Unbind()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// protected

FrameBufferObject::FrameBufferObject(int width, int height, const Format& format)
  : m_Error(false)
  , m_Height(height)
  , m_Width(width)
  , m_Format(format)
  , m_ColorRenderBuffer(nullptr)
  , m_DepthRenderBuffer(nullptr)
  , m_ColorTexture(nullptr)
  , m_DepthTexture(nullptr)
{
  // TODO: make util function
  GLint maxSamples;
  glGetIntegerv(GL_MAX_SAMPLES_EXT, &maxSamples);
  GLWarnUponError("in glGetIntegerv");
  
  if (format.samples > maxSamples) {
    m_Samples = maxSamples;
  } else if (format.samples <= 0) {
    m_Samples = 0;
  } else {
    m_Samples = format.samples;
  }
  
  // Framebuffer
  glGenFramebuffers(1, &m_FramebufferId);
  GLWarnUponError("in glGenFramebuffers");
  
  Bind();

  if (!initColor() || !checkStatus() || !initDepth() || !checkStatus()) {
    m_Error = true;
  }

  Unbind();
}

bool FrameBufferObject::checkStatus()
{
  m_Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  
  return m_Status == GL_FRAMEBUFFER_COMPLETE;
}

bool FrameBufferObject::initColor()
{
  if (m_Samples == 0) {
    GLTexture2Params params(m_Width, m_Height);
    params.SetInternalFormat(m_Format.internalColor);
    m_ColorTexture = new GLTexture2(params);
    
    // Attachment textures to FBO
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_ColorTexture->Id(), 0);
    GLWarnUponError("in glFramebufferTextureEXT");
  } else { // multisampling
    RenderBuffer::Format rbFormat;
    rbFormat.internalFormat = m_Format.internalColor;
    rbFormat.samples = m_Samples;
    m_ColorRenderBuffer = RenderBuffer::Create(m_Width, m_Height, rbFormat);
    if (m_ColorRenderBuffer == nullptr) {
      return false;
    } else {
      // Attachment textures to FBO
      glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, m_ColorRenderBuffer->Id());
      GLWarnUponError("in glFrameBufferRenderbufferEXT");
    }
  }
  
  return true;
}

bool FrameBufferObject::initDepth()
{
  if (!m_Format.depth) {
    return true;
  }
  
  if (m_Samples == 0) {
    GLTexture2Params params(m_Width, m_Height);
    params.SetInternalFormat(m_Format.internalDepth);
    m_DepthTexture = new GLTexture2(params);

    // Attachment textures to FBO
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, m_DepthTexture->Id(), 0);
    GLWarnUponError("in glFramebufferTexture2DEXT");
  } else { // multisampling
    RenderBuffer::Format rbFormat;
    rbFormat.internalFormat = m_Format.internalDepth;
    rbFormat.samples = m_Samples;
    m_DepthRenderBuffer = RenderBuffer::Create(m_Width, m_Height, rbFormat);
    if (m_DepthRenderBuffer == nullptr) {
      return false;
    } else {
      // Attachment textures to FBO
      glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_DepthRenderBuffer->Id());
      GLWarnUponError("in glFramebufferRenderbufferEXT");
    }
  }
  
  return true;
}
