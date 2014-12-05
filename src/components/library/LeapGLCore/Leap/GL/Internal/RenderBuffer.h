#pragma once

#include "Leap/GL/GLHeaders.h" // convenience header for cross-platform GL includes

namespace Leap {
namespace GL {
// The contents of the Internal namespace are not intended to be used publicly, and provide
// no guarantee as to the stability of their API.  The classes and functions are used
// internally in the implementation of the publicly-presented classes.
namespace Internal {

/// <summary>
/// The RenderBuffer class is a wrapper around the OpenGL Renderbuffer functionality
/// </summary>
class RenderBuffer
{
public:
  /// <summary>
  /// The Format struct has all of the parameters used to create the RenderBuffer
  /// </summary>
  struct Format {
    GLenum              internalFormat      = GL_RGBA;
    int                 samples             = 0;
  };

  /// <summary>
  /// Create a RenderBuffer
  /// </summary>
  static RenderBuffer* Create(int width, int height, const Format& format);

  /// <summary>
  /// Bind this RenderBuffer
  /// </summary>
  void Bind() const;

  /// <summary>
  /// Get the RenderBuffer ID
  /// </summary>
  GLuint Id() const;

  /// <summary>
  /// Unbind this Renderbuffer
  /// </summary>
  void Unbind() const;

protected:
  RenderBuffer(int width, int height, const Format& format);

  // No copy
  RenderBuffer(RenderBuffer const&) = delete;
  RenderBuffer& operator=(RenderBuffer const&) = delete;

  int                   m_Width;
  int                   m_Height;
  GLuint                m_RenderBufferId;
  GLint                 m_Samples;
};

} // end of namespace Internal
} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
