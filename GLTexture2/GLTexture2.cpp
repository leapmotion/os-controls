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
    std::stringstream ss;
    ss << "OpenGL error " << code << " " << while_doing;
    throw std::runtime_error(ss.str());
  }
}

// ////////////////////////////////////////////////////////////////////////////////////////////////
// GLTexture2Params
// ////////////////////////////////////////////////////////////////////////////////////////////////

GLTexture2Params::GLTexture2Params (GLsizei width, GLsizei height)
  :
  m_target(DEFAULT_TARGET),
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
// GLTexture2PixelData
// ////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: somehow make this less version-specific (?), or come up with a version-agnostic way
// to determine the size of each pixel from given pixel data format and type.
size_t GLTexture2PixelData::ComponentsInFormat (GLenum format) {
  // Allowable OpenGL 2.1 values: GL_COLOR_INDEX, GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_LUMINANCE, GL_LUMINANCE_ALPHA
  // Allowable OpenGL 3.3 values: GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL
  // Overlap between 2.1 and 3.3: GL_RED, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA
  // Only in OpenGL 2.1         : GL_COLOR_INDEX, GL_GREEN, GL_BLUE, GL_ALPHA, GL_LUMINANCE, GL_LUMINANCE_ALPHA
  // Only in OpenGL 3.3         : GL_RG, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL
  switch (format) {
    case GL_COLOR_INDEX:
    case GL_RED:
    case GL_GREEN:
    case GL_BLUE:
    case GL_ALPHA:
    case GL_LUMINANCE:
    case GL_DEPTH_COMPONENT:  return 1;

    case GL_LUMINANCE_ALPHA:
    case GL_RG:
    case GL_DEPTH_STENCIL:    return 2;

    case GL_RGB:
    case GL_BGR:              return 3;

    case GL_RGBA:
    case GL_BGRA:             return 4;

    default: throw std::invalid_argument("invalid pixel format; must be one of GL_COLOR_INDEX, GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_LUMINANCE, GL_LUMINANCE_ALPHA, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL");
  }
}

size_t GLTexture2PixelData::BytesInType (GLenum type) {
  // Allowable OpenGL 2.1 values: GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, GL_UNSIGNED_INT_2_10_10_10_REV, GL_BITMAP, 
  // Allowable OpenGL 3.3 values: GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, GL_UNSIGNED_INT_2_10_10_10_REV.
  // Overlap between 2.1 and 3.3: all but GL_BITMAP, which only occurs in OpenGL 2.1.  This one will not be supported for now.
  switch (type) {
    case GL_UNSIGNED_BYTE:
    case GL_BYTE:
    case GL_UNSIGNED_BYTE_3_3_2:
    case GL_UNSIGNED_BYTE_2_3_3_REV:      return 1;
      
    case GL_UNSIGNED_SHORT:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT_5_6_5:
    case GL_UNSIGNED_SHORT_5_6_5_REV:
    case GL_UNSIGNED_SHORT_4_4_4_4:
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:
    case GL_UNSIGNED_SHORT_5_5_5_1:
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:   return 2;
      
    case GL_UNSIGNED_INT:
    case GL_INT:
    case GL_FLOAT:
    case GL_UNSIGNED_INT_8_8_8_8:
    case GL_UNSIGNED_INT_8_8_8_8_REV:
    case GL_UNSIGNED_INT_10_10_10_2:
    case GL_UNSIGNED_INT_2_10_10_10_REV:  return 4;
      
    default: throw std::invalid_argument("invalid pixel type; must be one of GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, GL_UNSIGNED_INT_2_10_10_10_REV.");
  }
}

// ////////////////////////////////////////////////////////////////////////////////////////////////
// GLTexture2
// ////////////////////////////////////////////////////////////////////////////////////////////////

GLTexture2::GLTexture2 (const GLTexture2Params &params, const GLTexture2PixelData &pixel_data)
  :
  m_params(params)
{
  // Check the validity of the params.
  if (m_params.Width() == 0 || m_params.Height() == 0) {
    throw std::invalid_argument("GLTexture2Params must specify positive width and height"); // TODO: should this requirement be removed?
  }
  if (!pixel_data.IsEmpty() && (pixel_data.Format() == GL_INVALID_ENUM || pixel_data.Type() == GL_INVALID_ENUM)) {
    throw std::invalid_argument("GLTexture2PixelData must be empty or specify valid GLenum values for pixel data format and type");
  }
  // Check that the supplied data is the correct size.
  if (!pixel_data.IsEmpty() && pixel_data.RawDataByteCount() != GLTexture2PixelData::ComponentsInFormat(pixel_data.Format())*m_params.Width()*m_params.Height()) {
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

  glTexImage2D(m_params.Target(),
               0,                               // mipmap level (for source images, this should be 0)
               m_params.InternalFormat(),
               m_params.Width(),
               m_params.Height(),
               0,                               // border (must be 0)
               pixel_data.Format(),
               pixel_data.Type(),
               pixel_data.RawData());
  ThrowOnGLError("in glTexImage2D");

  // Retrieve and store the actual internal format that this GL implementation used for this texture.
  GLint actual_internal_format;
  glGetTexLevelParameteriv(m_params.Target(), 0, GL_TEXTURE_INTERNAL_FORMAT, &actual_internal_format);
  ThrowOnGLError("in glGetTexParameteriv");
  m_params.SetInternalFormat(actual_internal_format);

  // Unbind the texture to minimize the possibility that other GL calls may modify this texture.
  glBindTexture(m_params.Target(), 0);
}

GLTexture2::~GLTexture2 () {
  glDeleteTextures(1, &m_texture_name);
}
