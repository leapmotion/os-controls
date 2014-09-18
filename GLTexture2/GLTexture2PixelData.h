#pragma once

#include "gl_glext_glu.h" // convenience header for cross-platform GL includes
#include <stdexcept>
#include <map>
#include <vector>

// These are invaluable resources:
//   http://www.opengl.org/wiki/Common_Mistakes
//   http://www.opengl.org/wiki/Pixel_Transfer#Pixel_layout

// The OpenGL API website docs incorrectly formatted some of the formulas for glPixelStorei,
// but the MSDN website had them correctly formatted: http://msdn.microsoft.com/en-us/library/windows/desktop/dd940385(v=vs.85).aspx

// Base class for pixel data for use in all texel-loading operations in GLTexture2.
// Subclasses provide storage of and reference to pixel data.
class GLTexture2PixelData {
public:

  // Returns the number of components in format.  E.g. ComponentsInFormat(GL_RGBA) is 4.
  static size_t ComponentsInFormat (GLenum format);
  // Returns the number of bytes in the type.  E.g. BytesInType(GL_UNSIGNED_BYTE) is 1.
  static size_t BytesInType (GLenum type);

  typedef std::map<GLenum,GLint> GLPixelStoreiParameterMap;
  
  // The format and type values must be GL_INVALID_ENUM if RawData returns nullptr.
  GLTexture2PixelData (GLenum format, GLenum type);
  virtual ~GLTexture2PixelData () { }

  GLenum Format () const { return m_format; }
  GLenum Type () const { return m_type; }
  
  // Returns true if and only if this object refers to no data.
  virtual bool IsEmpty () const = 0;
  // Returns nullptr if this is not a readable pixel data object, otherwise a const "flattened"
  // array of data which will be interpreted in the way specified by Format() and Type().
  // "Flattened" means that all the components are contiguous in the array, e.g. RGBARGBARGBA...
  virtual const void *ReadableRawData () const = 0;
  // Returns nullptr if this is not a writeable pixel data object, otherwise a non-const
  // "flattened" array of data which will be interpreted in the way specified by Format() and
  // Type().  "Flattened" means that all the components are contiguous in the array, e.g. RGBARGBARGBA...
  virtual void *WriteableRawData () = 0;
  // Returns the number of bytes of data in the array returned by RawData.
  virtual size_t RawDataByteCount () const = 0;

  // Returns true iff pname exists in the PixelStorei parameter map (i.e. SetPixelStoreiParameter has been called with pname).
  bool HasPixelStoreiParameter (GLenum pname) const { return m_pixel_store_i_parameter.find(pname) != m_pixel_store_i_parameter.end(); }
  // Returns the value of the requested PixelStorei parameter mapping, or throws an exception if not set.
  GLint PixelStoreiParameter (GLenum pname) const;
  // Returns a const reference to the PixelStorei parameter map.
  const GLPixelStoreiParameterMap &PixelStoreiParameterMap () const { return m_pixel_store_i_parameter; }
  // Clears the PixelStorei parameter map.
  void ClearPixelStoreiParameterMap () { m_pixel_store_i_parameter.clear(); }

  // Record that the PixelStorei parameter given by pname will be set to the value given in param.
  void SetPixelStoreiParameter (GLenum pname, GLint param);

private:

  GLenum m_format;
  GLenum m_type;
  GLPixelStoreiParameterMap m_pixel_store_i_parameter;
};

// This class is used as a sentinel value for creating a GLTexture2 without uploading any pixel data.
class GLTexture2PixelDataEmpty : public GLTexture2PixelData {
public:

  GLTexture2PixelDataEmpty() : GLTexture2PixelData(GL_RGBA, GL_UNSIGNED_BYTE) { } // These are arbitrary but must be valid.
  virtual ~GLTexture2PixelDataEmpty() { }
  
  virtual bool IsEmpty () const override { return true; }
  virtual const void *ReadableRawData () const override { return nullptr; }
  virtual void *WriteableRawData () override { return nullptr; }
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

  // Construct this object with only a readable raw data pointer.  WriteableRawData will return nullptr.
  GLTexture2PixelDataReference (GLenum format, GLenum type, const void *readable_raw_pixel_data, size_t raw_pixel_data_byte_count);
  // Construct this object with a readable and writeable raw data pointer.  Both ReadableRawData
  // and WriteableRawData will return this pointer.
  GLTexture2PixelDataReference (GLenum format, GLenum type, void *readable_and_writeable_raw_pixel_data, size_t raw_pixel_data_byte_count);

  // Construct this object with only a readable pointer to the given std::vector.  WriteableRawData will return nullptr.
  template <typename Pixel_>
  GLTexture2PixelDataReference (GLenum format, GLenum type, const std::vector<Pixel_> &pixel_data)
    :
    GLTexture2PixelDataReference(format, type, static_cast<const void *>(pixel_data.data()), pixel_data.size()*sizeof(Pixel_))
  {
    if (ComponentsInFormat(format)*BytesInType(type) != sizeof(Pixel_)) {
      throw std::invalid_argument("the size of the Pixel_ type doesn't match the values of format and type");
    }
  }
  // Construct this object with a readable and writeable pointer to the given std::vector.  Both ReadableRawData
  // and WriteableRawData will return this pointer.
  template <typename Pixel_>
  GLTexture2PixelDataReference (GLenum format, GLenum type, std::vector<Pixel_> &pixel_data)
    :
    GLTexture2PixelDataReference(format, type, static_cast<void *>(pixel_data.data()), pixel_data.size()*sizeof(Pixel_))
  {
    if (ComponentsInFormat(format)*BytesInType(type) != sizeof(Pixel_)) {
      throw std::invalid_argument("the size of the Pixel_ type doesn't match the values of format and type");
    }
  }

  virtual bool IsEmpty () const override { return m_readable_raw_pixel_data == nullptr && m_writeable_raw_pixel_data == nullptr; }
  virtual const void *ReadableRawData () const override { return m_readable_raw_pixel_data; }
  virtual void *WriteableRawData () override { return m_writeable_raw_pixel_data; }
  virtual size_t RawDataByteCount () const override { return m_raw_pixel_data_byte_count; }
  
private:

  const void *m_readable_raw_pixel_data;
  void *m_writeable_raw_pixel_data;
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
    m_raw_pixel_count(raw_pixel_count),
    m_raw_pixels(m_raw_pixel_count)
  {
    assert(m_raw_pixels.size() == m_raw_pixel_count);
    // TODO: checks for validity in the type and format arguments?
    // TODO: check that Pixel_ is a POD of some type, and somehow check it against format and type.
    if (ComponentsInFormat(format)*BytesInType(type) != sizeof(Pixel_)) {
      throw std::invalid_argument("the size of the Pixel_ type doesn't match the values of format and type");
    }
  }

  // Const accessor for the vector of raw pixel data.
  const std::vector<Pixel_> &RawPixels () const { return m_raw_pixels; }
  // Non-const accessor for the vector of raw pixel data.  Do not modify its length; only its contents.
  std::vector<Pixel_> &RawPixels () { return m_raw_pixels; }

  // Returns true if and only if the raw pixel count is zero.
  virtual bool IsEmpty () const override { return m_raw_pixel_count == 0; }
  // Returns a pointer to the readable raw pixel data.  If the raw pixel vector has been altered in length (which
  // is a big no-no), this method will throw an exception.
  virtual const void *ReadableRawData () const override {
    // Ensure that the m_raw_pixels vector still has the correct size.
    if (m_raw_pixels.size() != m_raw_pixel_count) {
      throw std::runtime_error("The vector containing the raw pixel data has been resized, which is a prohibited operation");
    }
    return static_cast<const void *>(m_raw_pixels.data());
  }
  // Returns a pointer to the writeable raw pixel data.  If the raw pixel vector has been altered in length (which
  // is a big no-no), this method will throw an exception.
  virtual void *WriteableRawData () override {
    // Ensure that the m_raw_pixels vector still has the correct size.
    if (m_raw_pixels.size() != m_raw_pixel_count) {
      throw std::runtime_error("The vector containing the raw pixel data has been resized, which is a prohibited operation");
    }
    return static_cast<void *>(m_raw_pixels.data());
  }
  virtual size_t RawDataByteCount () const override { return m_raw_pixels.size()*sizeof(Pixel_); }

private:

  size_t m_raw_pixel_count;
  std::vector<Pixel_> m_raw_pixels;
};
