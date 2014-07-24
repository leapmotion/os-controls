#ifndef __Texture_H__
#define __Texture_H__

// TEMP: should make some sort of GLIncludes header
#include "SimGL.h"

#include <string>

struct FIBITMAP;

/// <summary>
/// The RenderBuffer class is a wrapper around the OpenGL Renderbuffer functionality.  Currently, multisampling is not
/// supported.
/// </summary>
class Texture
{
public:
  /// <summary>
  /// The Format struct has all of the parameters used to create the Texture class
  /// </summary>
  struct Format
  {
    // format the image is in
    GLenum              imageFormat         = GL_RGBA;

    // format to store the image in
    GLint               internalFormat      = GL_RGBA;

    // Mipmapping Level
    GLint               mipmapLevel         = 0;

    GLenum              type                = GL_UNSIGNED_BYTE;

    // Disabling texture multisampling because glTexImage2DMultisample fails on mac
    // Multisampling
    // GLsizei samples = 0;
    //
    // GLboolean fixedSampleLocations = GL_TRUE;
  };

  /// <summary>
  /// Create a Texture object
  /// </summary>
  static Texture* Create(const char * path, const Format& format);
  static Texture* Create(unsigned char * data, int width, int height, int bytesPerLine, int bpp, const Format& format);

  // Create an empty texture
  static Texture* Create(int width, int height, const Format& format);

  /// <summary>
  /// A static, one time initializer for the Texture classes.  This is a wrapper around the FreeImage_Initialize()
  /// method.
  /// </summary>
  static void Initialize();

  ~Texture();

  /// <summary>
  /// Bind this texture
  /// </summary>
  void Bind() const;

  /// <summary>
  /// Get the Texture ID
  /// </summary>
  GLuint Id() const;

  /// <summary>
  /// Unbind this texture
  /// </summary>
  void Unbind() const;

protected:
  Texture(int width, int height, const GLvoid* bits, const Format& format);

  // No copy
  Texture(Texture const&) = delete;
  Texture& operator=(Texture const&) = delete;

  GLuint                m_TextureId;
};

#endif
