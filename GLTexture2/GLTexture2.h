#pragma once

#include <map>
#include <vector>

#include "gl_glext_glu.h" // convenience header for cross-platform GL includes

// This is an invaluable resource: http://www.opengl.org/wiki/Common_Mistakes

// Notes for improvement/redesign of GLTexture2Params
//
// Design criteria:
// - Make a reasonably OpenGL-version-agnostic interface.  specifically,
//   avoid having specifically named properties unless they are necessary.
// - Use a minimal interface that covers as much configuration as is reasonable.
//
// Particular features to support:
// - Necessary parameters:
//   * target (e.g. GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X, etc)
//   * width
//   * height
//   * internal storage format (e.g. GL_RGBA8)
//   * pixel data type/format (indicating type of source data that is passed in)
// - Necessary parameters that have a reasonable default value (and therefore
//   could be set after construction):
//   * target (reasonable default is GL_TEXTURE_2D)
//   * internal storage format (arguably a reasonable default is GL_RGBA8)
// - Other parameters are set via glTexParameteri and glTexParameterf, and could be
//   specified by building a dictionary of parameter-name -> value mappings.  There
//   would need to be two dictionaries, one taking values in GLint, the other in
//   GLfloat.  Then the particular mappings are what are version-specific, but those
//   are specified by the application that uses this code, making this code version-
//   agnostic.

class GLTexture2Params {
public:

  typedef std::map<GLenum,GLfloat> GLTexParameterfMap;
  typedef std::map<GLenum,GLint> GLTexParameteriMap;

  // The necessary parameters that have reasonable defaults which are defined here.
  static const GLenum DEFAULT_TARGET          = GL_TEXTURE_2D;
  static const GLint  DEFAULT_INTERNAL_FORMAT = GL_RGBA8;

  // Construct a texture params object with the given paramers.
  // The target, pixel_data_format (format), pixel_data_type (type), internal_format parameters
  // are documented in the glTexImage2D docs; different versions:
  // OpenGL 2.1: https://www.opengl.org/sdk/docs/man2/
  // OpenGL 3.3: https://www.opengl.org/sdk/docs/man3/
  // More at http://www.opengl.org/wiki/Image_Formats though that document may reflect an OpenGL 
  // version later than 2.1.
  GLTexture2Params (GLsizei width,
                    GLsizei height,
                    GLenum pixel_data_format,
                    GLenum pixel_data_type,
                    GLenum internal_format = DEFAULT_INTERNAL_FORMAT);
  // Default constructor sets necessary values that have no reasonable default values to
  // invalid values which *must* be filled in.
  GLTexture2Params ()
    :
    GLTexture2Params(0, 0, GL_INVALID_ENUM, GL_INVALID_ENUM)
  { }

  // Accessors for glTexImage2D properties.

  GLenum Target () const { return m_target; }
  GLsizei Width () const { return m_size[0]; }
  GLsizei Height () const { return m_size[1]; }
  GLint InternalFormat () const { return m_internal_format; }
  GLenum PixelDataFormat () const { return m_pixel_data_format; }
  GLenum PixelDataType () const { return m_pixel_data_type; }
  /// @brief Convenience accessor for retrieving the size as any POD type having the same layout as GLint[2].
  template <typename T>
  const T &SizeAs () const {
    static_assert(sizeof(T) == sizeof(GLint)*2, "T_ must be a POD type consisting of exactly 2 GLint components");
    // TODO: somehow check that T_ actually is POD of the required variety.
    return *reinterpret_cast<const T *>(&m_size[0]);
  }

  // Accessors for glTexParameter* properties.

  bool HasTexParameterf (GLenum pname) const { return m_tex_parameter_f.find(pname) != m_tex_parameter_f.end(); }
  GLfloat TexParameterf (GLenum pname) const;
  const GLTexParameterfMap &TexParameterfMap () const { return m_tex_parameter_f; }
  bool HasTexParameteri (GLenum pname) const { return m_tex_parameter_i.find(pname) != m_tex_parameter_i.end(); }
  GLint TexParameteri (GLenum pname) const;
  const GLTexParameteriMap &TexParameteriMap () const { return m_tex_parameter_i; }

  // Modifiers for glTexImage2D properties.

  void SetTarget (GLenum target) { m_target = target; }
  void SetWidth (GLsizei width) { m_size[0] = width; }
  void SetHeight (GLsizei height) { m_size[1] = height; }
  void SetInternalFormat (GLint internal_format) { m_internal_format = internal_format; }
  void SetPixelDataFormat (GLenum pixel_data_format) { m_pixel_data_format = pixel_data_format; }
  void SetPixelDataType (GLenum pixel_data_type) { m_pixel_data_type = pixel_data_type; }

  // Modifiers for glTexParameter* properties.

  void SetTexParameterf (GLenum pname, GLfloat value);
  void SetTexParameteri (GLenum pname, GLint value);

private:

  GLenum m_target;
  GLsizei m_size[2];
  GLint m_internal_format;
  GLenum m_pixel_data_format;
  GLenum m_pixel_data_type;
  GLTexParameterfMap m_tex_parameter_f;
  GLTexParameteriMap m_tex_parameter_i;
};

// This class wraps creation and use of 2-dimensional GL textures.
class GLTexture2 {
public:

  // TODO: make GLTexture2-specific std::exception subclass?

  // Construct a GLTexture from raw data.  The pixel_data_byte_count should specify the number of bytes
  // in the array pointed to by pixel_data.   The PixelDataFormat and PixelDataType properties
  // of GLTexture2Params are what indicate what type of data pixel_data is.  An exception will
  // be thrown upon error.
  GLTexture2 (const GLTexture2Params &params, const void *pixel_data, size_t pixel_data_byte_count);
  // An exception will be thrown upon error.
  // Because this templatized function is defined in the implementation file, it is only
  // supported for particular types T_.  This is by design; the supported types are: GLbyte, 
  // GLubyte, GLshort, GLushort, GLint, GLuint, GLfloat.
  template <typename T_>
  GLTexture2 (const GLTexture2Params &params, const std::vector<T_> &pixel_data)
    :
    GLTexture2(params, reinterpret_cast<const void *>(pixel_data.data()), pixel_data.size()*sizeof(T_))
  {
    static_assert(is_same<T_,GLbyte>::value ||
                  is_same<T_,GLubyte>::value ||
                  is_same<T_,GLshort>::value ||
                  is_same<T_,GLushort>::value ||
                  is_same<T_,GLint>::value ||
                  is_same<T_,GLuint>::value ||
                  is_same<T_,GLfloat>::value,
                  "pixel data type T_ must be one of GLbyte, GLubyte, GLshort, GLushort, GLint, GLuint, GLfloat");
  }

  // Automatically frees the allocated resources.
  ~GLTexture2 ();

  // Returns the GLTexture2Params used to construct this texture.
  const GLTexture2Params &Params () const { return m_params; }
  
  // Returns the assigned GLuint generated by this texture
  GLuint Id () { return m_texture_name; }

  // This method should be called to bind this shader.
  void Bind () { glBindTexture(m_params.Target(), m_texture_name); }
  // This method should be called when no shader program should be used.
  void Unbind () { glBindTexture(m_params.Target(), 0); }

private:

  GLTexture2Params m_params;
  GLuint m_texture_name;
};
