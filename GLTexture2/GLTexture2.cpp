#include "GLTexture2.h"

#include "GLError.h"
#include <sstream>
#include <stdexcept>

// convenience macro for std::ostream style formatting expressions
#define FORMAT(expr) static_cast<std::ostringstream &>(std::ostringstream().flush() << expr).str()

namespace { // Anonymous namespace to hide this function from other compilation units.

// This function will call glPixelStorei on each of the pname:param key/value pairs in override_param_map,
// glGetIntegerv'ing the values of the overridden parameters beforehand, and storing them in overridden_param_map
// (which will first be cleared), for later restoring via RestorePixelStoreiParameters.
void OverridePixelStoreiParameters (const GLTexture2PixelData::GLPixelStoreiParameterMap &override_param_map,
                                    GLTexture2PixelData::GLPixelStoreiParameterMap &overridden_param_map) {
  overridden_param_map.clear();
  for (auto p : override_param_map) {
    GLint current_param;
    glGetIntegerv(p.first, &current_param);
    GLThrowUponError(FORMAT("in calling glGetIntegerv using pname = GLenum(0x" << std::hex << p.first << ')'));
    overridden_param_map[p.first] = current_param;
    glPixelStorei(p.first, p.second);
    GLThrowUponError(FORMAT("in setting glPixelStorei using pname = GLenum(0x" << std::hex << p.first << "), value = " << p.second));
  }
}

// This function will call glPixelStorei on each of the pname:param key/value pairs in overridden_param_map,
// which can be used to restore the PixelStorei values that were overridden in OverridePixelStoreiParameters.
void RestorePixelStoreiParameters (const GLTexture2PixelData::GLPixelStoreiParameterMap &overridden_param_map) {
  for (auto p : overridden_param_map)
  {
    glPixelStorei(p.first, p.second);
    GLThrowUponError(FORMAT("in setting glPixelStorei using pname = GLenum(0x" << std::hex << p.first << "), value = " << p.second));
  }
}

// Returns the ceiling of the ratio numerator/denominator.
template <typename IntType>
IntType CeilDiv (IntType numerator, IntType denominator) {
  return (numerator + denominator - 1) / denominator;
}

} // End of anonymous namespace.

GLTexture2::GLTexture2 (const GLTexture2Params &params, const GLTexture2PixelData &pixel_data)
  :
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
  for (auto p : m_params.TexParameterfMap())
  {
    glTexParameterf(m_params.Target(), p.first, p.second);
    GLThrowUponError(FORMAT("in setting glTexParameterf using pname = GLenum(0x" << std::hex << p.first << "), value = " << p.second));
  }
  // Set all the GLint texture parameters.
  for (auto p : m_params.TexParameteriMap())
  {
    glTexParameteri(m_params.Target(), p.first, p.second);
    GLThrowUponError(FORMAT("in setting glTexParameteri using pname = GLenum(0x" << std::hex << p.first << "), value = " << p.second));
  }
  // Store all the PixelStorei parameters that are about to be overridden, then override them.
  GLTexture2PixelData::GLPixelStoreiParameterMap overridden_pixel_store_i_parameter_map;
  OverridePixelStoreiParameters(pixel_data.PixelStoreiParameterMap(), overridden_pixel_store_i_parameter_map);
  
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

  // Restore the PixelStorei parameter values that were overridden above.
  RestorePixelStoreiParameters(overridden_pixel_store_i_parameter_map);
  
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

void GLTexture2::UpdateTexture(const GLTexture2PixelData &pixel_data) {
  VerifyPixelDataOrThrow(pixel_data);

  // Simply forward on to the subimage function.

  glBindTexture(m_params.Target(), m_texture_name);
  GLThrowUponError("in glBindTexture");

  // Store all the PixelStorei parameters that are about to be overridden, then override them.
  GLTexture2PixelData::GLPixelStoreiParameterMap overridden_pixel_store_i_parameter_map;
  OverridePixelStoreiParameters(pixel_data.PixelStoreiParameterMap(), overridden_pixel_store_i_parameter_map);
  
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

  // Restore the PixelStorei parameter values that were overridden above.
  RestorePixelStoreiParameters(overridden_pixel_store_i_parameter_map);

  glBindTexture(m_params.Target(), 0);
}

void GLTexture2::VerifyPixelDataOrThrow (const GLTexture2PixelData &pixel_data) const {
  // Ensure that the given data is valid and of the expected size
  if (!pixel_data.IsEmpty() && (pixel_data.Format() == GL_INVALID_ENUM || pixel_data.Type() == GL_INVALID_ENUM)) {
    throw std::invalid_argument("GLTexture2PixelData must be empty or specify valid GLenum values for pixel data format and type");
  }
  
  // Check that the supplied data is a valid size.  This is complicated slightly by the configurability
  // provided by glPixelStorei, as it affects how the pixel data is interpreted in the texel-loading operations.
  // In particular, the GL_UNPACK_ROW_LENGTH, GL_UNPACK_SKIP_PIXELS, and GL_UNPACK_SKIP_ROWS parameters, along
  // with the width and height of the texture, define a rectangular subregion of the image represented by the
  // pixel data which will be used in the texel-loading operation.  See http://opengl.czweb.org/ch11/358-361.html

  // The OpenGL API website docs incorrectly formatted some of the formulas for glPixelStorei, but the MSDN website
  // ( http://msdn.microsoft.com/en-us/library/windows/desktop/dd940385(v=vs.85).aspx ) has them correctly
  // formmatted, though they also have a copy-and-paste error and use GL_PACK_ROW_LENGTH and GL_PACK_ALIGNMENT
  // instead of the correct GL_UNPACK_ROW_LENGTH and GL_UNPACK_ALIGNMENT, as on the OpenGL API docs
  // ( https://www.opengl.org/sdk/docs/man2/ and http://www.opengl.org/sdk/docs/man3/ ).
  // 
  // If greater than zero, GL_UNPACK_ROW_LENGTH defines the number of pixels in a row. If the first pixel of a row is
  // placed at location p in memory, then the location of the first pixel of the next row is obtained by skipping
  // 
  //   k = { n*l                         if s >= a,
  //       { a/s * ceil(s*n*l/a)         if s < a
  //
  // components or indices, where n is the number of components or indices in a pixel, l is the number of pixels in
  // a row (GL_UNPACK_ROW_LENGTH if it is greater than 0, the width argument to the pixel routine otherwise), a is
  // the value of GL_UNPACK_ALIGNMENT, and s is the size, in bytes, of a single component (if a < s, then it is as
  // if a = s). In the case of 1-bit values, the location of the next row is obtained by skipping
  //
  //   k = 8*a * ceil(n*l/(8*a))
  //
  // components on indices.
  //
  // The word component in this description refers to the nonindex values red, green, blue, alpha, and depth. Storage
  // format GL_RGB, for example, has three components per pixel: first red, then green, and finally blue.
  
  if (m_params.Height() == 0) {
    return; // There is no data sufficiency to check, because there will be no data needed.
  }
  
  size_t n = GLTexture2PixelData::ComponentsInFormat(pixel_data.Format());  // Number of components in a pixel.
  size_t s = GLTexture2PixelData::BytesInType(pixel_data.Type());           // Number of bytes in a component.
  size_t l = pixel_data.HasPixelStoreiParameter(GL_UNPACK_ROW_LENGTH) ?     // Number of pixels in each row of data.
             pixel_data.PixelStoreiParameter(GL_UNPACK_ROW_LENGTH) :        // Use GL_UNPACK_ROW_LENGTH param if specified.
             m_params.Width();                                              // Otherwise the value is understood to be the texture width.
  size_t a = pixel_data.HasPixelStoreiParameter(GL_UNPACK_ALIGNMENT) ?      // Pixel unpacking alignment.
             pixel_data.PixelStoreiParameter(GL_UNPACK_ALIGNMENT) :         // Use GL_UNPACK_ALIGNMENT param if specified.
             4;                                                             // 4 is the default specified by OpenGL.
  // Don't bother with 1-bit values for now.
  size_t k = (s >= a) ?                                                     // k has units of component-count (number of components in row).
             n*l :                                                          // If k is divided by components per pixel, the units are pixel-count
             (CeilDiv(s*n*l, a)*a) / s;                                     // (number of pixels in a row).
  
  size_t skip_pixels = pixel_data.HasPixelStoreiParameter(GL_UNPACK_SKIP_PIXELS) ? pixel_data.PixelStoreiParameter(GL_UNPACK_SKIP_PIXELS) : 0; // 0 is the default specified by OpenGL.
  size_t skip_rows = pixel_data.HasPixelStoreiParameter(GL_UNPACK_SKIP_ROWS) ? pixel_data.PixelStoreiParameter(GL_UNPACK_SKIP_ROWS) : 0; // 0 is the default specified by OpenGL.
  size_t pixels_in_a_row = k/n;
  size_t starting_pixel_index = pixels_in_a_row*skip_rows + skip_pixels;
  size_t ending_pixel_index = starting_pixel_index + l*(m_params.Height()-1) + m_params.Width(); // The last row's data doesn't need to extend all the way to the theoretical next row.
  size_t sizeof_pixel = n*s;
  if (!pixel_data.IsEmpty() && pixel_data.RawDataByteCount() < ending_pixel_index*sizeof_pixel) {
    throw std::invalid_argument("there is insufficient pixel data for the given parameters");
  }
}
