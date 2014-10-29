#include "Leap/GL/GLTexture2PixelData.h"

#include <stdexcept>

namespace Leap {
namespace GL {

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

GLTexture2PixelData::GLTexture2PixelData (GLenum format, GLenum type)
  :
  m_format(format),
  m_type(type)
{
  // The following calls do checks for validity (basically checking that the format and type are each valid
  // in the sense that they're acceptable values for OpenGL 2.1 or OpenGL 3.3).
  size_t bytes_in_pixel = ComponentsInFormat(format)*BytesInType(type);
  // NOTE: TEMPORARY hacky handling of GL_UNPACK_ALIGNMENT, so that the assumption that all pixel
  // data is layed out contiguously in the raw pixel data is correct (it isn't necessarily, as
  // OpenGL has a row alignment feature, which depends on GL_UNPACK_ALIGNMENT).
  if (bytes_in_pixel % 4 != 0) {
    // The default is 4, so if our pixels don't align to 4 bytes, just hackily set it to 1.
    // This probably slows things down a bit, so TODO this should be re-engineered correctly later.
//     SetPixelStoreiParameter(GL_UNPACK_ALIGNMENT, 1);
    // If this value is overridden, it's assumed that the overrider knows what they're doing.
  }
}

GLint GLTexture2PixelData::PixelStoreiParameter (GLenum pname) const {
  // TODO: validate that pname is a valid argument for this function (see docs of glPixelStorei)
  auto it = m_pixel_store_i_parameter.find(pname);
  if (it == m_pixel_store_i_parameter.end()) {
    throw std::domain_error("specified GLint-valued PixelStorei parameter not found and/or specified");
  }
  return it->second;
}

void GLTexture2PixelData::SetPixelStoreiParameter (GLenum pname, GLint param) {
  // TODO: validate that pname is a valid argument for this function (see docs of glPixelStorei)
  m_pixel_store_i_parameter[pname] = param;
}

// ////////////////////////////////////////////////////////////////////////////////////////////////
// GLTexture2PixelDataReference
// ////////////////////////////////////////////////////////////////////////////////////////////////

GLTexture2PixelDataReference::GLTexture2PixelDataReference (GLenum format, GLenum type, const void *readable_raw_pixel_data, size_t raw_pixel_data_byte_count)
  :
  GLTexture2PixelData(format, type),
  m_readable_raw_pixel_data(readable_raw_pixel_data),
  m_writeable_raw_pixel_data(nullptr),
  m_raw_pixel_data_byte_count(raw_pixel_data_byte_count)
{
  if (readable_raw_pixel_data == nullptr && raw_pixel_data_byte_count > 0) {
    throw std::invalid_argument("if readable_raw_pixel_data is null, then raw_pixel_data_byte_count must be zero.");
  }
  // TODO: checks for validity in the type and format arguments?
}

GLTexture2PixelDataReference::GLTexture2PixelDataReference (GLenum format, GLenum type, void *readable_and_writeable_raw_pixel_data, size_t raw_pixel_data_byte_count)
  :
  GLTexture2PixelData(format, type),
  m_readable_raw_pixel_data(readable_and_writeable_raw_pixel_data),
  m_writeable_raw_pixel_data(readable_and_writeable_raw_pixel_data),
  m_raw_pixel_data_byte_count(raw_pixel_data_byte_count)
{
  if (readable_and_writeable_raw_pixel_data == nullptr && raw_pixel_data_byte_count > 0) {
    throw std::invalid_argument("if readable_and_writeable_raw_pixel_data is null, then raw_pixel_data_byte_count must be zero.");
  }
  // TODO: checks for validity in the type and format arguments?
}

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
