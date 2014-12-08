#pragma once

#include "Leap/GL/GLHeaders.h"
#include <string>

namespace Leap {
namespace GL {

// A C++ wrapper class for OpenGL buffer objects, of types e.g. GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER.
class Buffer {

public:

  Buffer();
  ~Buffer();
  void Create(GLenum type);
  void Bind() const;
  void Unbind () const;
  void Allocate(const void* data, GLsizeiptr size, GLenum usage_pattern);
  void Write(const void* data, int count);
  GLsizeiptr Size() const { return m_SizeInBytes; }
  void* Map(GLenum access);
  bool Unmap();
  bool IsCreated() const;
  void Destroy();

private:

  GLuint m_BufferAddress;
  GLenum m_BufferType;
  GLsizeiptr m_SizeInBytes;
};

} // end of namespace GL
} // end of namespace Leap

using namespace Leap::GL; // TEMPORARY until the Leap::GL:: scoping has been integrated into all code.
