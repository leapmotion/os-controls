#pragma once

#include "gl_glext_glu.h"
#include <string>

// A C++ wrapper class for OpenGL buffer objects, of types e.g. GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER.
class GLBuffer {

public:

  GLBuffer();
  ~GLBuffer();
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
