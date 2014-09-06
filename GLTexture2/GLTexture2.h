#pragma once

#include <map>
#include <vector>

#include "gl_glext_glu.h" // convenience header for cross-platform GL includes

// TEMPORARY shim until std::is_same from <type_traits> is available
template <typename T0_, typename T1_> struct is_same { static const bool value = false; };
template <typename T_> struct is_same<T_,T_> { static const bool value = true; };

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
  // The target, format (format), type (type), internal_format parameters
  // are documented in the glTexImage2D docs; different versions:
  // OpenGL 2.1: https://www.opengl.org/sdk/docs/man2/
  // OpenGL 3.3: https://www.opengl.org/sdk/docs/man3/
  // More at http://www.opengl.org/wiki/Image_Formats though that document may reflect an OpenGL 
  // version later than 2.1.
  GLTexture2Params (GLsizei width, GLsizei height);
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
  bool HasTexParameteri (GLenum pname) const { return m_tex_parameter_i.find(pname) != m_tex_parameter_i.end(); }
  GLint TexParameteri (GLenum pname) const;
  const GLTexParameteriMap &TexParameteriMap () const { return m_tex_parameter_i; }

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
  // TODO: rename to Format and Type
  // If IsEmpty() returns false, this returns the format of the pixel data (see the 'format'
  // parameter in glTexImage2D documentation).  If IsEmpty returns true, then this will return
  // an arbitrary value that is a valid parameter for the 'format' parameter to glTexImage2D.
  GLenum Format () const { return IsEmpty() ? GL_RGBA : m_format; }
  // If IsEmpty() returns false, this returns the type of the pixel data (see the 'type'
  // parameter in glTexImage2D documentation).  If IsEmpty returns true, then this will return
  // an arbitrary value that is a valid parameter for the 'type' parameter to glTexImage2D.
  GLenum Type () const { return IsEmpty() ? GL_UNSIGNED_BYTE : m_type; }

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

  GLTexture2PixelDataEmpty() : GLTexture2PixelData(GL_INVALID_ENUM, GL_INVALID_ENUM) { } // These are arbitrary, since IsEmpty() will return true.
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

// This class wraps creation and use of 2-dimensional GL textures.
class GLTexture2 {
public:
  GLTexture2(const GLTexture2& rhs) = delete;

  // TODO: make GLTexture2-specific std::exception subclass?

  // Construct a GLTexture2 with the specified parameters and pixel data. The pixel data is only
  // passed into glTexImage2D, and is not stored.  The default value for pixel_data is "empty",
  // which indicates that while the texture memory will be allocated for it, it will not be
  // initialized.  An exception will be thrown upon error.
  GLTexture2 (const GLTexture2Params &params, const GLTexture2PixelData &pixel_data = GLTexture2PixelDataEmpty());

  // Automatically frees the allocated resources.
  ~GLTexture2 ();

  // Returns the GLTexture2Params used to construct this texture.
  const GLTexture2Params &Params() const { return m_params; }

  // Updates the contents of this texture from the specified memory
  void UpdateTexture(const void* pMem);
  
  // Returns the assigned GLuint generated by this texture
  GLuint Id () { return m_texture_name; }

  // This method should be called to bind this shader.
  void Bind () { glBindTexture(m_params.Target(), m_texture_name); }
  // This method should be called when no shader program should be used.
  void Unbind () { glBindTexture(m_params.Target(), 0); }

private:
  GLTexture2Params m_params;
  const GLenum format;
  const GLenum type;
  GLuint m_texture_name;
};
