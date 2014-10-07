#pragma once

#include <map>

#include "gl_glext_glu.h" // convenience header for cross-platform GL includes

// This is an invaluable resource: http://www.opengl.org/wiki/Common_Mistakes

// Design notes for GLTexture2Params
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
  static const GLenum DEFAULT_INTERNAL_FORMAT = GL_RGBA8;

  // Construct a texture params object with the given paramers.
  // The target, format (format), type (type), internal_format parameters
  // are documented in the glTexImage2D docs; different versions:
  // OpenGL 2.1: https://www.opengl.org/sdk/docs/man2/
  // OpenGL 3.3: https://www.opengl.org/sdk/docs/man3/
  // More at http://www.opengl.org/wiki/Image_Formats though that document may reflect an OpenGL 
  // version later than 2.1.
  GLTexture2Params (GLsizei width, GLsizei height, GLenum internal_format = DEFAULT_INTERNAL_FORMAT);
  // Default constructor sets necessary values that have no reasonable default values to
  // invalid values which *must* be filled in.
  GLTexture2Params () : GLTexture2Params(0, 0) { }

  // Accessors for glTexImage2D properties.

  GLenum Target () const { return m_target; }
  GLsizei Width () const { return m_size[0]; }
  GLsizei Height () const { return m_size[1]; }
  GLint InternalFormat () const { return m_internal_format; }
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
  void ClearTexParameterfMap () { m_tex_parameter_f.clear(); }
  bool HasTexParameteri (GLenum pname) const { return m_tex_parameter_i.find(pname) != m_tex_parameter_i.end(); }
  GLint TexParameteri (GLenum pname) const;
  const GLTexParameteriMap &TexParameteriMap () const { return m_tex_parameter_i; }
  void ClearTexParameteriMap () { m_tex_parameter_i.clear(); }

  // Modifiers for glTexImage2D properties.

  void SetTarget (GLenum target) { m_target = target; }
  void SetWidth (GLsizei width) { m_size[0] = width; }
  void SetHeight (GLsizei height) { m_size[1] = height; }
  void SetInternalFormat (GLint internal_format) { m_internal_format = internal_format; }

  // Modifiers for glTexParameter* properties.

  void SetTexParameterf (GLenum pname, GLfloat value);
  void SetTexParameteri (GLenum pname, GLint value);

private:

  GLenum m_target;
  GLsizei m_size[2];
  GLint m_internal_format;
  GLTexParameterfMap m_tex_parameter_f;
  GLTexParameteriMap m_tex_parameter_i;
};

