#pragma once

#include "Leap/GL/GLHeaders.h"
#include <string>

namespace Leap {
namespace GL {

// A C++ wrapper class for OpenGL buffer objects, of types e.g. GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER.
class BufferObject {

public:

  BufferObject ();
  BufferObject (GLenum type);
  ~BufferObject ();

  void Initialize (GLenum type);
  void Shutdown ();

  bool IsInitialized () const { return m_BufferAddress != 0; }

  void Bind () const;
  void Unbind () const;

  void Allocate (const void* data, GLsizeiptr size, GLenum usage_pattern);
  void Write (const void* data, int count);
  GLsizeiptr Size () const { return m_SizeInBytes; }
  void* Map (GLenum access);
  bool Unmap ();

private:

  GLuint m_BufferAddress;
  GLenum m_BufferType;
  GLsizeiptr m_SizeInBytes;
};

} // end of namespace GL
} // end of namespace Leap
