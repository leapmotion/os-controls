#pragma once

#include "Leap/GL/GLHeaders.h" // convenience header for cross-platform GL includes
#include "Leap/GL/Texture2Exception.h"
#include <unordered_map>

// These are invaluable resources:
//   http://www.opengl.org/wiki/Common_Mistakes
//   http://www.opengl.org/wiki/Pixel_Transfer#Pixel_layout

// The OpenGL API website docs incorrectly formatted some of the formulas for glPixelStorei,
// but the MSDN website had them correctly formatted: http://msdn.microsoft.com/en-us/library/windows/desktop/dd940385(v=vs.85).aspx

namespace Leap {
namespace GL {

// Base class for pixel data for use in all texel-loading operations in Texture2.
// Subclasses provide storage of and reference to pixel data. The only exceptions that this
// class explicitly throws derive from Leap::GL::Texture2Exception.
class Texture2PixelData {
public:

  // Returns the number of components in format.  E.g. ComponentsInFormat(GL_RGBA) is 4.
  static size_t ComponentsInFormat (GLenum format);
  // Returns the number of bytes in the type.  E.g. BytesInType(GL_UNSIGNED_BYTE) is 1.
  static size_t BytesInType (GLenum type);

  typedef std::unordered_map<GLenum,GLint> GLPixelStoreiParameterMap;

  // Specifies an IsEmpty pixel data structure.  Default, valid format and type values are assigned, but they
  // aren't used by Texture2 if IsEmpty returns is true.  This indicates that OpenGL should allocate texture
  // memory automatically.
  Texture2PixelData ();
  // Specifies a pixel format/type and read-only pixel data.  The pointer must be non-null, and
  // raw_data_byte_count must be positive.  The way the data is interpreted by OpenGL
  // depends on the pixel store parameters (see glPixelStorei).
  Texture2PixelData (GLenum format, GLenum type, const void *readable_raw_data, size_t raw_data_byte_count);
  // Specifies a pixel format/type and pixel data that is readable and writable.  The pointer must
  // be non-null, and raw_data_byte_count must be positive.  The way the data is interpreted by OpenGL
  // depends on the pixel store parameters (see glPixelStorei).
  Texture2PixelData (GLenum format, GLenum type, void *readable_and_writeable_raw_data, size_t raw_data_byte_count);

  GLenum Format () const { return m_format; }
  GLenum Type () const { return m_type; }
  // Returns true if and only if this object refers to no data.
  bool IsEmpty () const { return m_raw_data_byte_count == 0; }
  bool IsReadable () const { return m_readable_raw_data != nullptr; }
  bool IsWriteable () const { return m_writeable_raw_data != nullptr; }
  // Returns a non-null pointer if this IsReadable, otherwise returns nullptr.
  const void *ReadableRawData () const { return m_readable_raw_data; }
  // Returns a non-null pointer if this IsWriteable, otherwise returns nullptr.
  void *WriteableRawData () const { return m_writeable_raw_data; }
  // If IsReadable or IsWriteable, returns the number of bytes of data in the array returned
  // by each of ReadableRawData and WriteableRawData (whichever one is relevant), otherwise
  // returns 0.
  size_t RawDataByteCount () const { return m_raw_data_byte_count; }

  // Returns true iff pname exists in the PixelStorei parameter map (i.e. SetPixelStoreiParameter has been called with pname).
  bool HasPixelStoreiParameter (GLenum pname) const { return m_pixel_store_i_parameter.find(pname) != m_pixel_store_i_parameter.end(); }
  // Returns the value of the requested PixelStorei parameter mapping, or throws Texture2Exception if not set.
  GLint PixelStoreiParameter (GLenum pname) const;
  // Returns a const reference to the PixelStorei parameter map.
  const GLPixelStoreiParameterMap &PixelStoreiParameterMap () const { return m_pixel_store_i_parameter; }

  // Sets the format and type of the pixel data.  This will throw Texture2Exception if either argument
  // is not valid.
  void SetFormatAndType (GLenum format, GLenum type);
  // Clears the raw data pointers and byte count, thereby making this IsEmpty.  Does not modify the format/type or pixel store params.
  void MakeEmpty ();
  // Sets a readable raw pixel data pointer (making IsReadable() true and IsWriteable() false).
  void MakeReadable (const void *readable_raw_data, size_t raw_data_byte_count);
  // Sets a readable-and-writable raw pixel data pointer (making IsReadable() and IsWriteable() both true).
  void MakeReadableAndWriteable (void *readable_and_writable_raw_pixel_data, size_t raw_data_byte_count);
  // Record that the PixelStorei parameter given by pname will be set to the value given in param.
  void SetPixelStoreiParameter (GLenum pname, GLint param);
  // Clears the PixelStorei parameter map.
  void ClearPixelStoreiParameterMap () { m_pixel_store_i_parameter.clear(); }

private:

  GLenum m_format;
  GLenum m_type;
  const void *m_readable_raw_data;
  void *m_writeable_raw_data;
  size_t m_raw_data_byte_count;
  GLPixelStoreiParameterMap m_pixel_store_i_parameter;
};

} // end of namespace GL
} // end of namespace Leap
