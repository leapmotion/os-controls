#pragma once

#include <stdexcept>
#include <vector>

#include "gl_glext_glu.h" // convenience header for cross-platform GL includes

// TEMPORARY shim until std::is_same from <type_traits> is available
template <typename T0_, typename T1_> struct is_same { static const bool value = false; };
template <typename T_> struct is_same<T_,T_> { static const bool value = true; };

// This is an invaluable resource: http://www.opengl.org/wiki/Common_Mistakes

// Base class for pixel data for use in all texel-loading operations in GLTexture2.
// Subclasses provide storage of and reference to pixel data.
class GLTexture2PixelData {
public:

  // The format and type values must be GL_INVALID_ENUM if RawData returns nullptr.
  GLTexture2PixelData (GLenum format, GLenum type)
    :
    m_format(format),
    m_type(type)
  {
    // TODO: checks for validity
  }
  virtual ~GLTexture2PixelData () { }

  bool IsEmpty () const { return RawData() == nullptr; }
  GLenum Format () const { return m_format; }
  GLenum Type () const { return m_type; }

  // Returns a "flattened" array of data which will be interpreted in the way specified
  // by Format() and Type().  "Flattened" means that all the components are contiguous in
  // the array, e.g. RGBARGBARGBA...
  virtual const void *RawData () const = 0;
  // Returns the number of bytes of data in the array returned by RawData.
  virtual size_t RawDataByteCount () const = 0;

  static size_t ComponentsInFormat (GLenum format);
  static size_t BytesInType (GLenum type);
  
private:

  GLenum m_format;
  GLenum m_type;
};

// This class is used as a sentinel value for creating a GLTexture2 without uploading any pixel data.
class GLTexture2PixelDataEmpty : public GLTexture2PixelData {
public:

  GLTexture2PixelDataEmpty() : GLTexture2PixelData(GL_RGBA, GL_UNSIGNED_BYTE) { } // These are arbitrary but must be valid.
  virtual ~GLTexture2PixelDataEmpty() { }
  
  virtual const void *RawData () const override { return nullptr; }
  virtual size_t RawDataByteCount () const override { return 0; }
};

// This class is used in the texel-loading operations to specify pixel data in GLTexture2.
// An instance of this class may not live beyond the lifetime of the data it is referencing
// (hence the "Reference" part of the class' name).  The main use case is for passing pixel
// already-allocated data to texel-loading operations in GLTexture2.  The meaning of
// constructing a GLTexture2 with a nullptr value for raw_pixel_data is documented in
// glTexImage2D; texture memory is allocated but left uninitialized.
class GLTexture2PixelDataReference : public GLTexture2PixelData {
public:

  GLTexture2PixelDataReference (GLenum format, GLenum type, const void *raw_pixel_data, size_t raw_pixel_data_byte_count)
    :
    GLTexture2PixelData(format, type),
    m_raw_pixel_data(raw_pixel_data),
    m_raw_pixel_data_byte_count(raw_pixel_data_byte_count)
  {
    if (raw_pixel_data == nullptr && raw_pixel_data_byte_count > 0) {
      throw std::invalid_argument("if raw_pixel_data is null, then raw_pixel_data_byte_count must be zero.");
    }
    // TODO: checks for validity in the type and format arguments?
  }
  template <typename PixelComponent_>
  GLTexture2PixelDataReference (GLenum format, GLenum type, const std::vector<PixelComponent_> &pixel_data)
    :
    GLTexture2PixelDataReference(format, type, reinterpret_cast<const void *>(pixel_data.data()), pixel_data.size()*sizeof(PixelComponent_))
  {
    static_assert(is_same<PixelComponent_,GLbyte>::value ||
                  is_same<PixelComponent_,GLubyte>::value ||
                  is_same<PixelComponent_,GLshort>::value ||
                  is_same<PixelComponent_,GLushort>::value ||
                  is_same<PixelComponent_,GLint>::value ||
                  is_same<PixelComponent_,GLuint>::value ||
                  is_same<PixelComponent_,GLfloat>::value,
                  "PixelComponent_ must be one of GLbyte, GLubyte, GLshort, GLushort, GLint, GLuint, GLfloat");
  }

  virtual const void *RawData () const override { return m_raw_pixel_data; }
  virtual size_t RawDataByteCount () const override { return m_raw_pixel_data_byte_count; }
  
private:

  const void *m_raw_pixel_data;
  size_t m_raw_pixel_data_byte_count;
};

// This class is used in the texel-loading operations to specify pixel data in GLTexture2.
// An instance of this class creates a std::vector<Pixel_>, where Pixel_ is the template
// parameter to this class.  This class should be used when you would like memory for pixel
// data to be allocated/deallocated for you.
template <typename Pixel_>
class GLTexture2PixelDataStorage : public GLTexture2PixelData {
public:
  
  GLTexture2PixelDataStorage (GLenum format, GLenum type, size_t raw_pixel_count)
    :
    GLTexture2PixelData(format, type),
    m_raw_pixels(raw_pixel_count)
  {
    // TODO: checks for validity in the type and format arguments?
    // TODO: check that Pixel_ is a POD of some type, and somehow check it against format and type.
    if (ComponentsInFormat(format)*BytesInType(type) != sizeof(Pixel_)) {
      throw std::invalid_argument("the size of the Pixel_ type doesn't match the values of format and type");
    }
  }

  const std::vector<Pixel_> &RawPixels () const { return m_raw_pixels; }
  std::vector<Pixel_> &RawPixels () { return m_raw_pixels; }
  
  virtual const void *RawData () const override { return m_raw_pixels.data(); }
  virtual size_t RawDataByteCount () const override { return m_raw_pixels.size()*sizeof(Pixel_); }

private:

  std::vector<Pixel_> m_raw_pixels;
};
