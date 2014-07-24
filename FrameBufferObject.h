#ifndef __FrameBufferObject_H__
#define __FrameBufferObject_H__

#include "SimGL.h"

class RenderBuffer;
class Texture;

/// <summary>
/// The FrameBufferObject class is a wrapper around the OpenGL Framebuffer functionality.  The class will create a
/// FrameBufferObject and attach a color (Texture or RenderBuffer) and an optional depth (Texture or RenderBuffer).
/// If multisampling is enabled (Format.samples > 0), then the attachments will be attached as a RenderBuffer,
/// otherwise, the attachments will be attached as a Texture
/// </summary>
class FrameBufferObject
{
public:
  /// <summary>
  /// The Format struct has all of the parameters used to create the FrameBufferObject
  /// </summary>
  struct Format
  {
    // Color
    GLenum              internalColor = GL_RGBA;
    
    // Depth
    bool                depth         = false;
    GLenum              internalDepth = GL_DEPTH_COMPONENT;

    // Multisampling
    GLsizei             samples       = 0;
  };

  /// <summary>
  /// Create a FrameBufferObject
  /// </summary>
  static FrameBufferObject* Create(int width, int height, const Format& format);
  
  /// <summary>
  /// Blit two FrameBufferObjects
  /// </summary>
  static void Blit(FrameBufferObject* source, FrameBufferObject* target, GLbitfield buffers, GLenum filter);
  
public:
  /// <summary>
  /// Destructor
  /// </summary>
  ~FrameBufferObject();

  /// <summary>
  /// Bind this FrameBufferObject
  /// </summary>
  void Bind();
  
  /// <summary>
  /// Get the Color Texture.  This function may return null (i.e. uses RenderBuffer for multisampling)
  /// </summary>
  Texture* ColorTexture();

  bool HasError() const;

  int Height() const;

  GLuint Id() const;

  int Width() const;

  GLenum Status() const;

  /// <summary>
  /// Bind this FrameBufferObject
  /// </summary>
  void Unbind();

protected:
  FrameBufferObject(int width, int height, const Format& format);

  // No copy
  FrameBufferObject(FrameBufferObject const&) = delete;
  FrameBufferObject& operator=(FrameBufferObject const&) = delete;

  bool checkStatus();
  
  bool initColor();
  bool initDepth();

  bool                  m_Error;
  int                   m_Height;
  int                   m_Width;
  int                   m_Samples;
  Format                m_Format;

  GLuint                m_FramebufferId;
  GLenum                m_Status;

  RenderBuffer*         m_ColorRenderBuffer;
  RenderBuffer*         m_DepthRenderBuffer;
  
  Texture*              m_ColorTexture;
  Texture*              m_DepthTexture;
};

#endif
