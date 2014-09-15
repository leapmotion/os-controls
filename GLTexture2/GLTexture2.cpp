#include "GLTexture2.h"

#include "GLError.h"
#include <sstream>
#include <stdexcept>

// convenience macro for std::ostream style formatting expressions
#define FORMAT(expr) static_cast<std::ostringstream &>(std::ostringstream().flush() << expr).str()

GLTexture2::GLTexture2 (const GLTexture2Params &params, const GLTexture2PixelData &pixel_data)
  :
  m_format(pixel_data.Format()),
  m_type(pixel_data.Type()),
  m_params(params)
{
  // Check the validity of the params.
  if (m_params.Width() == 0 || m_params.Height() == 0) {
    throw std::invalid_argument("GLTexture2Params must specify positive width and height"); // TODO: should this requirement be removed?
  }
  VerifyPixelDataOrThrow(pixel_data);

  // Clear the GL error flag in case it was not cleared from some other unrelated GL operation
  GLClearError();
  glGenTextures(1, &m_texture_name);
  GLThrowUponError("in glGenTextures");
  glBindTexture(m_params.Target(), m_texture_name);
  GLThrowUponError("in glBindTexture");

  // Set all the GLfloat texture parameters.
  for (GLTexture2Params::GLTexParameterfMap::const_iterator it = m_params.TexParameterfMap().begin();
       it != m_params.TexParameterfMap().end();
       ++it)
  {
    glTexParameterf(m_params.Target(), it->first, it->second);
    GLThrowUponError(FORMAT("in setting glTexParameterf using pname = GLenum(0x" << std::hex << it->first << "), value = " << it->second));
  }
  // Set all the GLint texture parameters.
  for (GLTexture2Params::GLTexParameteriMap::const_iterator it = m_params.TexParameteriMap().begin();
       it != m_params.TexParameteriMap().end();
       ++it)
  {
    glTexParameteri(m_params.Target(), it->first, it->second);
    GLThrowUponError(FORMAT("in setting glTexParameteri using pname = GLenum(0x" << std::hex << it->first << "), value = " << it->second));
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
  GLThrowUponError("in glTexImage2D");

  // Retrieve and store the actual internal format that this GL implementation used for this texture.
  GLint actual_internal_format;
  glGetTexLevelParameteriv(m_params.Target(), 0, GL_TEXTURE_INTERNAL_FORMAT, &actual_internal_format);
  GLThrowUponError("in glGetTexParameteriv");
  m_params.SetInternalFormat(actual_internal_format);

  // Unbind the texture to minimize the possibility that other GL calls may modify this texture.
  glBindTexture(m_params.Target(), 0);
}

GLTexture2::~GLTexture2 () {
  glDeleteTextures(1, &m_texture_name);
}

void GLTexture2::UpdateTexture(const void *data) {
  // Simply forward on to the subimage function.

  glBindTexture(m_params.Target(), m_texture_name);
  GLThrowUponError("in glBindTexture");

  glTexSubImage2D(
    m_params.Target(),
    0,
    0,
    0,
    m_params.Width(),
    m_params.Height(),
    m_format,
    m_type,
    data
  );
  GLThrowUponError("in glTexSubImage2D");
 
  glBindTexture(m_params.Target(), 0);
}

void GLTexture2::UpdateTexture(const GLTexture2PixelData &pixel_data) {
  VerifyPixelDataOrThrow(pixel_data);

  // Simply forward on to the subimage function.

  glBindTexture(m_params.Target(), m_texture_name);
  GLThrowUponError("in glBindTexture");

  glTexSubImage2D(
    m_params.Target(),
    0,
    0,
    0,
    m_params.Width(),
    m_params.Height(),
    pixel_data.Format(),
    pixel_data.Type(),
    pixel_data.RawData()
  );
  GLThrowUponError("in glTexSubImage2D");
 
  glBindTexture(m_params.Target(), 0);
}

void GLTexture2::VerifyPixelDataOrThrow (const GLTexture2PixelData &pixel_data) const {
  // Ensure that the given data is valid and of the expected size
  if (!pixel_data.IsEmpty() && (pixel_data.Format() == GL_INVALID_ENUM || pixel_data.Type() == GL_INVALID_ENUM)) {
    throw std::invalid_argument("GLTexture2PixelData must be empty or specify valid GLenum values for pixel data format and type");
  }
  // Check that the supplied data is the correct size.
  size_t pixel_count = m_params.Width()*m_params.Height();
  size_t sizeof_pixel = GLTexture2PixelData::ComponentsInFormat(pixel_data.Format())*GLTexture2PixelData::BytesInType(pixel_data.Type());
  size_t expected_raw_data_byte_count = pixel_count*sizeof_pixel;
  if (!pixel_data.IsEmpty() && pixel_data.RawDataByteCount() != expected_raw_data_byte_count) {
    throw std::invalid_argument("the number of components in pixel_data did not correspond to width*height");
  }
}
