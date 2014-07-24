#include "GLTexture2.h"

#include <ios>
#include <sstream>
#include <stdexcept>
#include <string>

// convenience macro for std::ostream style formatting expressions
#define FORMAT(expr) static_cast<std::ostringstream &>(std::ostringstream().flush() << expr).str()

void ThrowOnGLError (const std::string &while_doing) {
  GLenum code = glGetError();
  if (code != GL_NO_ERROR) {
    throw std::runtime_error("OpenGL error \"" + std::string(reinterpret_cast<const char *>(gluErrorString(code))) + "\" " + while_doing);
  }
}

// ////////////////////////////////////////////////////////////////////////////////////////////////
// GLTexture2Params
// ////////////////////////////////////////////////////////////////////////////////////////////////

GLTexture2Params::GLTexture2Params (
  GLsizei width,
  GLsizei height,
  GLenum pixel_data_format,
  GLenum pixel_data_type)
  :
  m_target(DEFAULT_TARGET),
  m_pixel_data_format(pixel_data_format),
  m_pixel_data_type(pixel_data_type),
  m_internal_format(DEFAULT_INTERNAL_FORMAT)
{ 
  m_size[0] = width;
  m_size[1] = height;
}

GLfloat GLTexture2Params::TexParameterf (GLenum pname) const {
  GLTexParameterfMap::const_iterator it = m_tex_parameter_f.find(pname);
  if (it == m_tex_parameter_f.end()) {
    throw std::domain_error("specified GLfloat-valued texture parameter not found and/or specified");
  }
  return it->second;
}

GLint GLTexture2Params::TexParameteri (GLenum pname) const {
  GLTexParameteriMap::const_iterator it = m_tex_parameter_i.find(pname);
  if (it == m_tex_parameter_i.end()) {
    throw std::domain_error("specified GLint-valued texture parameter not found and/or specified");
  }
  return it->second;
}

void GLTexture2Params::SetTexParameterf (GLenum pname, GLfloat value) {
  if (m_tex_parameter_i.find(pname) != m_tex_parameter_i.end()) {
    throw std::domain_error("specified GLfloat-valued texture parameter already set in the GLint-valued parameters");
  }
  m_tex_parameter_f[pname] = value;
}
void GLTexture2Params::SetTexParameteri (GLenum pname, GLint value) {
  if (m_tex_parameter_f.find(pname) != m_tex_parameter_f.end()) {
    throw std::domain_error("specified GLint-valued texture parameter already set in the GLfloat-valued parameters");
  }
  m_tex_parameter_i[pname] = value;
}

// ////////////////////////////////////////////////////////////////////////////////////////////////
// GLTexture2
// ////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: somehow make this less version-specific (?), or come up with a version-agnostic way
// to determine the size of each pixel from given pixel data format and type.
size_t ComponentsInPixelFormat (GLenum format) {
  switch (format) {
    case GL_COLOR_INDEX:
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_ALPHA:
    case GL_LUMINANCE:        return 1;

    case GL_LUMINANCE_ALPHA:  return 2;

    case GL_RGB:
    case GL_BGR:              return 3;

    case GL_RGBA:
    case GL_BGRA:             return 4;

    default: throw std::invalid_argument("invalid pixel format; must be one of GL_COLOR_INDEX, GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_LUMINANCE, and GL_LUMINANCE_ALPHA");
  }
}

GLTexture2::GLTexture2 (const GLTexture2Params &params, const void *pixel_data, size_t pixel_data_byte_count)
  :
  m_params(params)
{
  // Check the validity of the params.
  if (m_params.Width() == 0 || m_params.Height() == 0) {
    throw std::invalid_argument("GLTexture2Params must specify positive width and height");
  }
  if (m_params.PixelDataFormat() == GL_INVALID_ENUM || m_params.PixelDataType() == GL_INVALID_ENUM) {
    throw std::invalid_argument("GLTexture2Params must specify valid GLenum values for pixel data format and type");
  }
  // Check that the supplied data is the correct size.
  if (pixel_data_byte_count != ComponentsInPixelFormat(m_params.PixelDataFormat())*m_params.Width()*m_params.Height()) {
    throw std::invalid_argument("the number of components in pixel_data did not correspond to width*height");
  }
  glGenTextures(1, &m_texture_name);
  ThrowOnGLError("in glGenTextures");
  glBindTexture(m_params.Target(), m_texture_name);
  ThrowOnGLError("in glBindTexture");

  // Set all the GLfloat texture parameters.
  for (GLTexture2Params::GLTexParameterfMap::const_iterator it = m_params.TexParameterfMap().begin();
       it != m_params.TexParameterfMap().end();
       ++it)
  {
    glTexParameterf(m_params.Target(), it->first, it->second);
    ThrowOnGLError(FORMAT("in setting glTexParameterf using pname = GLenum(0x" << std::hex << it->first << "), value = " << it->second));
  }
  // Set all the GLint texture parameters.
  for (GLTexture2Params::GLTexParameteriMap::const_iterator it = m_params.TexParameteriMap().begin();
       it != m_params.TexParameteriMap().end();
       ++it)
  {
    glTexParameteri(m_params.Target(), it->first, it->second);
    ThrowOnGLError(FORMAT("in setting glTexParameteri using pname = GLenum(0x" << std::hex << it->first << "), value = " << it->second));
  }

  // glTexParameteri(m_params.Target(), GL_TEXTURE_MIN_FILTER, m_params.TextureMinFilter());
  // ThrowOnGLError("in setting glTexParameteri for GL_TEXTURE_MIN_FILTER");
  // glTexParameteri(m_params.Target(), GL_TEXTURE_MAG_FILTER, m_params.TextureMagFilter());
  // ThrowOnGLError("in setting glTexParameteri for GL_TEXTURE_MAG_FILTER");
  // // NOTE: GL_GENERATE_MIPMAP is deprecated/removed in 3.0/3.1, in preference of glGenerateMipmaps(m_params.Target())
  // glTexParameteri(m_params.Target(), GL_GENERATE_MIPMAP, GL_TRUE); 
  // ThrowOnGLError("in setting glTexParameteri for GL_GENERATE_MIPMAP");

  glTexImage2D(m_params.Target(),
               0,                               // mipmap level (for source images, this should be 0)
               m_params.InternalFormat(),
               m_params.Width(),
               m_params.Height(),
               0,                               // border (must be 0)
               m_params.PixelDataFormat(),
               m_params.PixelDataType(),
               pixel_data);
  ThrowOnGLError("in glTexImage2D");

  // Retrieve and store the actual internal format that this GL implementation used for this texture.
  GLint actual_internal_format;
  glGetTexLevelParameteriv(m_params.Target(), 0, GL_TEXTURE_INTERNAL_FORMAT, &actual_internal_format);
  ThrowOnGLError("in glGetTexParameteriv");
  m_params.SetInternalFormat(actual_internal_format);  
}

template <typename T_>
GLTexture2::GLTexture2 (const GLTexture2Params &params, const std::vector<T_> &pixel_data)
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

GLTexture2::~GLTexture2 () {
  glDeleteTextures(1, &m_texture_name);
}

namespace {

// hack to do explicit instantiations of the constructor GLTexture2::GLTexture2<T_>.
// don't call this function.
void instantiate_particular_versions (const GLTexture2Params &params) {
  { GLTexture2 t(params, std::vector<GLbyte>()); }
  { GLTexture2 t(params, std::vector<GLubyte>()); }
  { GLTexture2 t(params, std::vector<GLshort>()); }
  { GLTexture2 t(params, std::vector<GLushort>()); }
  { GLTexture2 t(params, std::vector<GLint>()); }
  { GLTexture2 t(params, std::vector<GLuint>()); }
  { GLTexture2 t(params, std::vector<GLfloat>()); }
}

} // end of anonymous
