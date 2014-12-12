#pragma once

#include "Leap/GL/GLHeaders.h"
#include "Leap/GL/ResourceBase.h"
#include <string>

namespace Leap {
namespace GL {

/// @brief A C++ abstraction for the buffer object concept (untyped storage of data) in OpenGL.
/// @details This is a low-level class that is used by other higher-level classes such as
/// VertexBuffer.
/// @see https://www.opengl.org/wiki/Buffer_Object
class BufferObject : public ResourceBase<BufferObject> {
public:

  /// @brief Construct an un-Initialize-d BufferObject which has not acquired any GL (or other) resources.
  /// @details It will be necessary to call Initialize on this object to use it.
  BufferObject ();
  /// @brief Convenience constructor that will call Initialize with the given arguments.
  BufferObject (GLenum type);
  /// @brief Destructor will call Shutdown.
  ~BufferObject ();

  using ResourceBase<BufferObject>::IsInitialized;
  using ResourceBase<BufferObject>::Initialize;
  using ResourceBase<BufferObject>::Shutdown;

  void Bind () const;
  void Unbind () const;

  void Allocate (const void* data, GLsizeiptr size, GLenum usage_pattern);
  void Write (const void* data, int count);
  GLsizeiptr Size () const { return m_SizeInBytes; }
  void* Map (GLenum access);
  bool Unmap ();

private:

  friend class ResourceBase<BufferObject>;

  /// @brief Returns true iff this object has been Initialize-d.
  bool IsInitialized_Implementation () const { return m_BufferAddress != 0; }
  /// @brief Initialize this buffer object 
  void Initialize_Implementation (GLenum type);
  /// @brief Releases any the GL resources acquired by this buffer object.
  void Shutdown_Implementation ();

  GLuint m_BufferAddress;
  GLenum m_BufferType;
  GLsizeiptr m_SizeInBytes;
};

} // end of namespace GL
} // end of namespace Leap
